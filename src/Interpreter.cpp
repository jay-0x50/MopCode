#include "Interpreter.h"

#include "Lexer.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>

namespace mopcode
{
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

    return executeFunction("main", functions);
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

        const std::string name = tokens[current].lexeme;
        ++current;

        if (!expect(tokens, current, TokenType::LeftParen, "expected '(' after function name")
            || !expect(tokens, current, TokenType::RightParen, "expected ')' after function parameters")
            || !expect(tokens, current, TokenType::Colon, "expected ':' before return type")
            || !expect(tokens, current, TokenType::Void, "v0.3 only supports void functions")
            || !expect(tokens, current, TokenType::LeftBrace, "expected function body"))
        {
            return false;
        }

        std::vector<Token> body;
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
                    break;
                }
            }

            if (braceDepth > 0)
            {
                body.push_back(tokens[current]);
                ++current;
            }
        }

        if (braceDepth != 0)
        {
            printError("expected '}' after function body");
            return false;
        }

        functions[name] = Function{name, body};
    }

    return true;
}

bool Interpreter::executeFunction(const std::string& name, const std::unordered_map<std::string, Function>& functions) const
{
    const auto function = functions.find(name);
    if (function == functions.end())
    {
        printError("function not found: " + name);
        return false;
    }

    return executeBody(function->second.body, functions);
}

bool Interpreter::executeBody(const std::vector<Token>& body, const std::unordered_map<std::string, Function>& functions) const
{
    std::size_t current = 0;
    while (current < body.size())
    {
        if (body[current].type != TokenType::Identifier)
        {
            printError("expected statement");
            return false;
        }

        Value result;
        if (!parseCall(body, current, functions, result))
        {
            return false;
        }
    }

    return true;
}

bool Interpreter::parseCall(
    const std::vector<Token>& tokens,
    std::size_t& current,
    const std::unordered_map<std::string, Function>& functions,
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
            if (!parseExpression(tokens, current, functions, argument))
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

    return executeCall(name, arguments, functions, result);
}

bool Interpreter::parseExpression(
    const std::vector<Token>& tokens,
    std::size_t& current,
    const std::unordered_map<std::string, Function>& functions,
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
        return parseCall(tokens, current, functions, result);
    default:
        printError("expected expression");
        return false;
    }
}

bool Interpreter::executeCall(
    const std::string& name,
    const std::vector<Value>& arguments,
    const std::unordered_map<std::string, Function>& functions,
    Value& result) const
{
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
        if (arguments.size() != 2
            || arguments[0].type() != Value::Type::Number
            || arguments[1].type() != Value::Type::Number)
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

        static std::mt19937 engine(std::random_device{}());
        std::uniform_int_distribution<int> distribution(min, max);
        result = Value(static_cast<double>(distribution(engine)));
        return true;
    }

    if (name == "RandomFloat")
    {
        if (arguments.size() != 2
            || arguments[0].type() != Value::Type::Number
            || arguments[1].type() != Value::Type::Number)
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

        static std::mt19937 engine(std::random_device{}());
        std::uniform_real_distribution<double> distribution(min, max);
        result = Value(distribution(engine));
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

    const auto function = functions.find(name);
    if (function == functions.end())
    {
        printError("unknown function: " + name);
        return false;
    }

    if (!arguments.empty())
    {
        printError("user functions do not support parameters yet");
        return false;
    }

    result = Value();
    return executeBody(function->second.body, functions);
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
