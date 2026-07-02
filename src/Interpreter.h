#pragma once

#include "Token.h"
#include "Value.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace mopcode
{
class Interpreter
{
public:
    bool runFile(const std::string& path) const;

private:
    struct Function
    {
        struct Parameter
        {
            std::string name;
            TokenType type;
        };

        std::string name;
        std::vector<Parameter> parameters;
        std::vector<Token> body;
    };

    using Environment = std::unordered_map<std::string, Value>;

    bool executeSource(const std::string& source) const;
    bool collectFunctions(const std::vector<Token>& tokens, std::unordered_map<std::string, Function>& functions) const;
    bool executeFunction(
        const std::string& name,
        const std::vector<Value>& arguments,
        const std::unordered_map<std::string, Function>& functions,
        Value& result) const;
    bool executeBody(
        const std::vector<Token>& body,
        const std::unordered_map<std::string, Function>& functions,
        Environment& environment) const;
    bool executeStatement(
        const std::vector<Token>& body,
        std::size_t& current,
        const std::unordered_map<std::string, Function>& functions,
        Environment& environment) const;
    bool executeIf(
        const std::vector<Token>& body,
        std::size_t& current,
        const std::unordered_map<std::string, Function>& functions,
        Environment& environment) const;
    bool executeFor(
        const std::vector<Token>& body,
        std::size_t& current,
        const std::unordered_map<std::string, Function>& functions,
        Environment& environment) const;
    bool executeCall(
        const std::string& name,
        const std::vector<Value>& arguments,
        const std::unordered_map<std::string, Function>& functions,
        Environment& environment,
        Value& result) const;
    bool parseCall(
        const std::vector<Token>& tokens,
        std::size_t& current,
        const std::unordered_map<std::string, Function>& functions,
        Environment& environment,
        Value& result) const;
    bool parseExpression(
        const std::vector<Token>& tokens,
        std::size_t& current,
        const std::unordered_map<std::string, Function>& functions,
        Environment& environment,
        Value& result) const;
    bool parseComparison(
        const std::vector<Token>& tokens,
        std::size_t& current,
        const std::unordered_map<std::string, Function>& functions,
        Environment& environment,
        Value& result) const;
    bool parseTerm(
        const std::vector<Token>& tokens,
        std::size_t& current,
        const std::unordered_map<std::string, Function>& functions,
        Environment& environment,
        Value& result) const;
    bool parseFactor(
        const std::vector<Token>& tokens,
        std::size_t& current,
        const std::unordered_map<std::string, Function>& functions,
        Environment& environment,
        Value& result) const;
    bool parsePrimary(
        const std::vector<Token>& tokens,
        std::size_t& current,
        const std::unordered_map<std::string, Function>& functions,
        Environment& environment,
        Value& result) const;
    static bool readBlock(const std::vector<Token>& tokens, std::size_t& current, std::vector<Token>& block);
    static bool typesMatch(TokenType type, const Value& value);
    static bool isTypeToken(TokenType type);
    static bool expect(const std::vector<Token>& tokens, std::size_t& current, TokenType type, const std::string& message);
    static void printError(const std::string& message);
};
}
