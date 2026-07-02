#pragma once

#include "Token.h"

#include <string>
#include <vector>

namespace mopcode
{
class Lexer
{
public:
    explicit Lexer(std::string source);

    std::vector<Token> scanTokens();
    bool hadError() const;

private:
    bool isAtEnd() const;
    char advance();
    char peek() const;
    char peekNext() const;
    bool match(char expected);

    void scanToken();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& lexeme);
    void scanIdentifier();
    void scanNumber();
    void scanString();
    void reportError(const std::string& message);

    std::string source_;
    std::vector<Token> tokens_;
    std::size_t start_ = 0;
    std::size_t current_ = 0;
    int line_ = 1;
    int column_ = 1;
    int tokenLine_ = 1;
    int tokenColumn_ = 1;
    bool hadError_ = false;
};
}
