#include "Interpreter.h"

#include "Lexer.h"

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>

namespace mopcode
{
namespace
{
std::mt19937& randomEngine()
{
    static const auto seed = static_cast<unsigned int>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
    static std::mt19937 engine(seed);
    return engine;
}

bool isNumber(const Value& value)
{
    return value.type() == Value::Type::Number;
}
}

bool Interpreter::runFile(const std::string& path) const
{
    std::ifstream file(path);
    if (!file)
    {
        std::cerr << "MopCode Error: file not found: " << path << '\n';
        return false;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    return executeSource(buffer.str());
}

bool Interpreter::executeSource(const std::string& source) const
{
    Lexer lexer(source);
    const auto tokens = lexer.scanTokens();
    if (lexer.hadError())
    {
        return false;
    }

    std::unordered_map<std::string, Function> functions;
    if (!collectFunctions(tokens, functions))
    {
        return false;
    }

    Value result;
    return executeFunction("main", {}, functions, result);
}

bool Interpreter::collectFunctions(const std::vector<Token>& tokens, std::unordered_map<std::string, Function>& functions) const
{
    std::size_t current = 0;
    while (current < tokens.size() && tokens[current].type != TokenType::EndOfFile)
    {
        if (!expect(tokens, current, TokenType::Function, "expected function declaration"))
        {
            return false;
        }

        if (current >= tokens.size() || tokens[current].type != TokenType::Identifier)
        {
            printError("expected function name");
            return false;
        }

        Function function;
        function.name = tokens[current].lexeme;
        ++current;

        if (!expect(tokens, current, TokenType::LeftParen, "expected '(' after function name"))
        {
            return false;
        }

        if (current < tokens.size() && tokens[current].type != TokenType::RightParen)
        {
            while (true)
            {
                Function::Parameter parameter;

                if (current < tokens.size() && isTypeToken(tokens[current].type))
                {
                    parameter.type = tokens[current].type;
                    ++current;

                    if (current >= tokens.size() || tokens[current].type != TokenType::Identifier)
                    {
                        printError("expected parameter name");
                        return false;
                    }
                    parameter.name = tokens[current].lexeme;
                    ++current;
                }
                else
                {
                    if (current >= tokens.size() || tokens[current].type != TokenType::Identifier)
                    {
                        printError("expected parameter name");
                        return false;
                    }
                    parameter.name = tokens[current].lexeme;
                    ++current;

                    if (!expect(tokens, current, TokenType::Colon, "expected ':' after parameter name"))
                    {
                        return false;
                    }

                    if (current >= tokens.size() || !isTypeToken(tokens[current].type))
                    {
                        printError("expected parameter type");
                        return false;
                    }
                    parameter.type = tokens[current].type;
                    ++current;
                }

                function.parameters.push_back(parameter);

                if (current >= tokens.size() || tokens[current].type != TokenType::Comma)
                {
                    break;
                }
                ++current;
            }
        }

        if (!expect(tokens, current, TokenType::RightParen, "expected ')' after function parameters")
            || !expect(tokens, current, TokenType::Colon, "expected ':' before return type")
            || !expect(tokens, current, TokenType::Void, "v0.4 only supports void functions"))
        {
            return false;
        }

        if (!readBlock(tokens, current, function.body))
        {
            return false;
        }

        functions[function.name] = function;
    }

    return true;
}

bool Interpreter::executeFunction(
    const std::string& name,
    const std::vector<Value>& arguments,
    const std::unordered_map<std::string, Function>& functions,
    Value& result) const
{
    const auto function = functions.find(name);
    if (function == functions.end())
    {
        printError("function not found: " + name);
        return false;
    }

    if (arguments.size() != function->second.parameters.size())
    {
        printError("wrong argument count for function: " + name);
        return false;
    }

    Environment environment;
    for (std::size_t i = 0; i < arguments.size(); ++i)
    {
        const auto& parameter = function->second.parameters[i];
        if (!typesMatch(parameter.type, arguments[i]))
        {
            printError("argument type mismatch for parameter: " + parameter.name);
            return false;
        }
        environment[parameter.name] = arguments[i];
    }

    result = Value();
    return executeBody(function->second.body, functions, environment);
}

bool Interpreter::executeBody(
    const std::vector<Token>& body,
    const std::unordered_map<std::string, Function>& functions,
    Environment& environment) const
{
    std::size_t current = 0;
    while (current < body.size())
    {
        if (!executeStatement(body, current, functions, environment))
        {
            return false;
        }
    }

    return true;
}

bool Interpreter::executeStatement(
    const std::vector<Token>& body,
    std::size_t& current,
    const std::unordered_map<std::string, Function>& functions,
    Environment& environment) const
{
    if (current >= body.size())
    {
        return true;
    }

    if (body[current].type == TokenType::If)
    {
        return executeIf(body, current, functions, environment);
    }

    if (body[current].type == TokenType::For)
    {
        return executeFor(body, current, functions, environment);
    }

    if (body[current].type != TokenType::Identifier)
    {
        printError("expected statement");
        return false;
    }

    Value result;
    return parseCall(body, current, functions, environment, result);
}

bool Interpreter::executeIf(
    const std::vector<Token>& body,
    std::size_t& current,
    const std::unordered_map<std::string, Function>& functions,
    Environment& environment) const
{
    ++current;
    Value condition;
    if (!parseExpression(body, current, functions, environment, condition))
    {
        return false;
    }

    std::vector<Token> thenBlock;
    if (!readBlock(body, current, thenBlock))
    {
        return false;
    }

    std::vector<Token> elseBlock;
    bool hasElse = false;
    if (current < body.size() && body[current].type == TokenType::Else)
    {
        ++current;
        hasElse = true;
        if (!readBlock(body, current, elseBlock))
        {
            return false;
        }
    }

    if (condition.isTruthy())
    {
        return executeBody(thenBlock, functions, environment);
    }

    if (hasElse)
    {
        return executeBody(elseBlock, functions, environment);
    }

    return true;
}

bool Interpreter::executeFor(
    const std::vector<Token>& body,
    std::size_t& current,
    const std::unordered_map<std::string, Function>& functions,
    Environment& environment) const
{
    ++current;
    if (current >= body.size() || body[current].type != TokenType::Identifier)
    {
        printError("expected loop variable after for");
        return false;
    }

    const std::string variableName = body[current].lexeme;
    ++current;

    if (!expect(body, current, TokenType::Assignment, "expected '=' after loop variable"))
    {
        return false;
    }

    Value start;
    if (!parseExpression(body, current, functions, environment, start)
        || !expect(body, current, TokenType::Comma, "expected ',' after loop start"))
    {
        return false;
    }

    Value end;
    if (!parseExpression(body, current, functions, environment, end))
    {
        return false;
    }

    std::vector<Token> loopBlock;
    if (!readBlock(body, current, loopBlock))
    {
        return false;
    }

    if (!isNumber(start) || !isNumber(end))
    {
        printError("for loop bounds must be numbers");
        return false;
    }

    const int first = static_cast<int>(start.asNumber());
    const int last = static_cast<int>(end.asNumber());
    const int step = first <= last ? 1 : -1;
    const bool hadPrevious = environment.find(variableName) != environment.end();
    const Value previous = hadPrevious ? environment[variableName] : Value();

    for (int value = first; step > 0 ? value <= last : value >= last; value += step)
    {
        environment[variableName] = Value(static_cast<double>(value));
        if (!executeBody(loopBlock, functions, environment))
        {
            return false;
        }
    }

    if (hadPrevious)
    {
        environment[variableName] = previous;
    }
    else
    {
        environment.erase(variableName);
    }

    return true;
}

bool Interpreter::parseCall(
    const std::vector<Token>& tokens,
    std::size_t& current,
    const std::unordered_map<std::string, Function>& functions,
    Environment& environment,
    Value& result) const
{
    if (current >= tokens.size() || tokens[current].type != TokenType::Identifier)
    {
        printError("expected function call");
        return false;
    }

    const std::string name = tokens[current].lexeme;
    ++current;

    if (!expect(tokens, current, TokenType::LeftParen, "expected '(' after function call"))
    {
        return false;
    }

    std::vector<Value> arguments;
    if (current < tokens.size() && tokens[current].type != TokenType::RightParen)
    {
        while (true)
        {
            Value argument;
            if (!parseExpression(tokens, current, functions, environment, argument))
            {
                return false;
            }
            arguments.push_back(argument);

            if (current >= tokens.size() || tokens[current].type != TokenType::Comma)
            {
                break;
            }
            ++current;
        }
    }

    if (!expect(tokens, current, TokenType::RightParen, "expected ')' after function call"))
    {
        return false;
    }

    return executeCall(name, arguments, functions, environment, result);
}

bool Interpreter::parseExpression(
    const std::vector<Token>& tokens,
    std::size_t& current,
    const std::unordered_map<std::string, Function>& functions,
    Environment& environment,
    Value& result) const
{
    return parseComparison(tokens, current, functions, environment, result);
}

bool Interpreter::parseComparison(
    const std::vector<Token>& tokens,
    std::size_t& current,
    const std::unordered_map<std::string, Function>& functions,
    Environment& environment,
    Value& result) const
{
    if (!parseTerm(tokens, current, functions, environment, result))
    {
        return false;
    }

    while (current < tokens.size())
    {
        const TokenType operation = tokens[current].type;
        if (operation != TokenType::Less && operation != TokenType::Greater
            && operation != TokenType::LessEqual && operation != TokenType::GreaterEqual
            && operation != TokenType::EqualEqual && operation != TokenType::BangEqual)
        {
            break;
        }

        ++current;
        Value right;
        if (!parseTerm(tokens, current, functions, environment, right))
        {
            return false;
        }

        bool comparison = false;
        if (operation == TokenType::EqualEqual || operation == TokenType::BangEqual)
        {
            comparison = result.toString() == right.toString();
            if (operation == TokenType::BangEqual)
            {
                comparison = !comparison;
            }
        }
        else
        {
            if (!isNumber(result) || !isNumber(right))
            {
                printError("comparison operators require numbers");
                return false;
            }

            if (operation == TokenType::Less)
            {
                comparison = result.asNumber() < right.asNumber();
            }
            else if (operation == TokenType::Greater)
            {
                comparison = result.asNumber() > right.asNumber();
            }
            else if (operation == TokenType::LessEqual)
            {
                comparison = result.asNumber() <= right.asNumber();
            }
            else if (operation == TokenType::GreaterEqual)
            {
                comparison = result.asNumber() >= right.asNumber();
            }
        }

        result = Value(comparison);
    }

    return true;
}

bool Interpreter::parseTerm(
    const std::vector<Token>& tokens,
    std::size_t& current,
    const std::unordered_map<std::string, Function>& functions,
    Environment& environment,
    Value& result) const
{
    if (!parseFactor(tokens, current, functions, environment, result))
    {
        return false;
    }

    while (current < tokens.size() && (tokens[current].type == TokenType::Plus || tokens[current].type == TokenType::Minus))
    {
        const TokenType operation = tokens[current].type;
        ++current;
        Value right;
        if (!parseFactor(tokens, current, functions, environment, right))
        {
            return false;
        }

        if (operation == TokenType::Plus && (result.type() == Value::Type::String || right.type() == Value::Type::String))
        {
            result = Value(result.toString() + right.toString());
        }
        else
        {
            if (!isNumber(result) || !isNumber(right))
            {
                printError("math operators require numbers");
                return false;
            }
            result = Value(operation == TokenType::Plus
                ? result.asNumber() + right.asNumber()
                : result.asNumber() - right.asNumber());
        }
    }

    return true;
}

bool Interpreter::parseFactor(
    const std::vector<Token>& tokens,
    std::size_t& current,
    const std::unordered_map<std::string, Function>& functions,
    Environment& environment,
    Value& result) const
{
    if (!parsePrimary(tokens, current, functions, environment, result))
    {
        return false;
    }

    while (current < tokens.size() && (tokens[current].type == TokenType::Star || tokens[current].type == TokenType::Slash))
    {
        const TokenType operation = tokens[current].type;
        ++current;
        Value right;
        if (!parsePrimary(tokens, current, functions, environment, right))
        {
            return false;
        }

        if (!isNumber(result) || !isNumber(right))
        {
            printError("math operators require numbers");
            return false;
        }

        if (operation == TokenType::Slash && right.asNumber() == 0.0)
        {
            printError("division by zero");
            return false;
        }

        result = Value(operation == TokenType::Star
            ? result.asNumber() * right.asNumber()
            : result.asNumber() / right.asNumber());
    }

    return true;
}

bool Interpreter::parsePrimary(
    const std::vector<Token>& tokens,
    std::size_t& current,
    const std::unordered_map<std::string, Function>& functions,
    Environment& environment,
    Value& result) const
{
    if (current >= tokens.size())
    {
        printError("expected expression");
        return false;
    }

    const Token& token = tokens[current];
    switch (token.type)
    {
    case TokenType::StringLiteral:
        result = Value(token.lexeme);
        ++current;
        return true;
    case TokenType::IntegerLiteral:
    case TokenType::FloatLiteral:
        result = Value(std::atof(token.lexeme.c_str()));
        ++current;
        return true;
    case TokenType::True:
        result = Value(true);
        ++current;
        return true;
    case TokenType::False:
        result = Value(false);
        ++current;
        return true;
    case TokenType::Identifier:
    {
        if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::LeftParen)
        {
            return parseCall(tokens, current, functions, environment, result);
        }

        const auto variable = environment.find(token.lexeme);
        if (variable == environment.end())
        {
            printError("unknown variable: " + token.lexeme);
            return false;
        }

        result = variable->second;
        ++current;
        return true;
    }
    case TokenType::LeftParen:
        ++current;
        if (!parseExpression(tokens, current, functions, environment, result))
        {
            return false;
        }
        return expect(tokens, current, TokenType::RightParen, "expected ')' after expression");
    default:
        printError("expected expression");
        return false;
    }
}

bool Interpreter::executeCall(
    const std::string& name,
    const std::vector<Value>& arguments,
    const std::unordered_map<std::string, Function>& functions,
    Environment& environment,
    Value& result) const
{
    (void)environment;

    if (name == "Print")
    {
        if (arguments.size() != 1)
        {
            printError("Print expects 1 argument");
            return false;
        }
        std::cout << arguments[0].toString() << '\n';
        result = Value();
        return true;
    }

    if (name == "Log")
    {
        if (arguments.size() != 1)
        {
            printError("Log expects 1 argument");
            return false;
        }
        std::cout << "[Log] " << arguments[0].toString() << '\n';
        result = Value();
        return true;
    }

    if (name == "DeltaTime")
    {
        if (!arguments.empty())
        {
            printError("DeltaTime expects 0 arguments");
            return false;
        }
        result = Value(0.016);
        return true;
    }

    if (name == "RandomInt")
    {
        if (arguments.size() != 2 || !isNumber(arguments[0]) || !isNumber(arguments[1]))
        {
            printError("RandomInt expects 2 number arguments");
            return false;
        }

        const int min = static_cast<int>(arguments[0].asNumber());
        const int max = static_cast<int>(arguments[1].asNumber());
        if (min > max)
        {
            printError("RandomInt min must be less than or equal to max");
            return false;
        }

        std::uniform_int_distribution<int> distribution(min, max);
        result = Value(static_cast<double>(distribution(randomEngine())));
        return true;
    }

    if (name == "RandomFloat")
    {
        if (arguments.size() != 2 || !isNumber(arguments[0]) || !isNumber(arguments[1]))
        {
            printError("RandomFloat expects 2 number arguments");
            return false;
        }

        const double min = arguments[0].asNumber();
        const double max = arguments[1].asNumber();
        if (min > max)
        {
            printError("RandomFloat min must be less than or equal to max");
            return false;
        }

        std::uniform_real_distribution<double> distribution(min, max);
        result = Value(distribution(randomEngine()));
        return true;
    }

    if (name == "Spawn")
    {
        if (arguments.size() != 1 || arguments[0].type() != Value::Type::String)
        {
            printError("Spawn expects 1 string argument");
            return false;
        }
        std::cout << "[Game] Spawn " << arguments[0].asString() << '\n';
        result = Value();
        return true;
    }

    if (name == "SetState")
    {
        if (arguments.size() != 1 || arguments[0].type() != Value::Type::String)
        {
            printError("SetState expects 1 string argument");
            return false;
        }
        std::cout << "[Game] State " << arguments[0].asString() << '\n';
        result = Value();
        return true;
    }

    return executeFunction(name, arguments, functions, result);
}

bool Interpreter::readBlock(const std::vector<Token>& tokens, std::size_t& current, std::vector<Token>& block)
{
    if (!expect(tokens, current, TokenType::LeftBrace, "expected block body"))
    {
        return false;
    }

    int braceDepth = 1;
    while (current < tokens.size() && tokens[current].type != TokenType::EndOfFile && braceDepth > 0)
    {
        if (tokens[current].type == TokenType::LeftBrace)
        {
            ++braceDepth;
        }
        else if (tokens[current].type == TokenType::RightBrace)
        {
            --braceDepth;
            if (braceDepth == 0)
            {
                ++current;
                return true;
            }
        }

        if (braceDepth > 0)
        {
            block.push_back(tokens[current]);
            ++current;
        }
    }

    printError("expected '}' after block");
    return false;
}

bool Interpreter::typesMatch(TokenType type, const Value& value)
{
    switch (type)
    {
    case TokenType::Int:
    case TokenType::Float:
        return value.type() == Value::Type::Number;
    case TokenType::String:
        return value.type() == Value::Type::String;
    case TokenType::Bool:
        return value.type() == Value::Type::Bool;
    default:
        return false;
    }
}

bool Interpreter::isTypeToken(TokenType type)
{
    return type == TokenType::Int
        || type == TokenType::Float
        || type == TokenType::String
        || type == TokenType::Bool;
}

bool Interpreter::expect(const std::vector<Token>& tokens, std::size_t& current, TokenType type, const std::string& message)
{
    if (current >= tokens.size() || tokens[current].type != type)
    {
        printError(message);
        return false;
    }

    ++current;
    return true;
}

void Interpreter::printError(const std::string& message)
{
    std::cerr << "MopCode Error: " << message << '\n';
}
}
