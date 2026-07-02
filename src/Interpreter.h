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
        std::string name;
        std::vector<Token> body;
    };

    bool executeSource(const std::string& source) const;
    bool collectFunctions(const std::vector<Token>& tokens, std::unordered_map<std::string, Function>& functions) const;
    bool executeFunction(const std::string& name, const std::unordered_map<std::string, Function>& functions) const;
    bool executeBody(const std::vector<Token>& body, const std::unordered_map<std::string, Function>& functions) const;
    bool executeCall(
        const std::string& name,
        const std::vector<Value>& arguments,
        const std::unordered_map<std::string, Function>& functions,
        Value& result) const;
    bool parseCall(
        const std::vector<Token>& tokens,
        std::size_t& current,
        const std::unordered_map<std::string, Function>& functions,
        Value& result) const;
    bool parseExpression(
        const std::vector<Token>& tokens,
        std::size_t& current,
        const std::unordered_map<std::string, Function>& functions,
        Value& result) const;
    static bool expect(const std::vector<Token>& tokens, std::size_t& current, TokenType type, const std::string& message);
    static void printError(const std::string& message);
};
}
