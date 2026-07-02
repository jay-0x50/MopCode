#include "Lexer.h"

#include <cctype>
#include <iostream>
#include <unordered_map>
#include <utility>

namespace mopcode
{
namespace
{
const std::unordered_map<std::string, TokenType> keywords = {
    {"function", TokenType::Function},
    {"return", TokenType::Return},
    {"if", TokenType::If},
    {"else", TokenType::Else},
    {"for", TokenType::For},
    {"true", TokenType::True},
    {"false", TokenType::False},
    {"int", TokenType::Int},
    {"float", TokenType::Float},
    {"string", TokenType::String},
    {"bool", TokenType::Bool},
    {"void", TokenType::Void},
};

bool isIdentifierStart(char value)
{
    return std::isalpha(static_cast<unsigned char>(value)) || value == '_';
}

bool isIdentifierPart(char value)
{
    return std::isalnum(static_cast<unsigned char>(value)) || value == '_';
}
}

Lexer::Lexer(std::string source)
    : source_(std::move(source))
{
}

std::vector<Token> Lexer::scanTokens()
{
    while (!isAtEnd())
    {
        start_ = current_;
        tokenLine_ = line_;
        tokenColumn_ = column_;
        scanToken();
    }

    tokens_.push_back({TokenType::EndOfFile, "", line_, column_});
    return tokens_;
}

bool Lexer::hadError() const
{
    return hadError_;
}

bool Lexer::isAtEnd() const
{
    return current_ >= source_.size();
}

char Lexer::advance()
{
    const char value = source_[current_++];
    if (value == '\n')
    {
        ++line_;
        column_ = 1;
    }
    else
    {
        ++column_;
    }

    return value;
}

char Lexer::peek() const
{
    if (isAtEnd())
    {
        return '\0';
    }

    return source_[current_];
}

char Lexer::peekNext() const
{
    if (current_ + 1 >= source_.size())
    {
        return '\0';
    }

    return source_[current_ + 1];
}

bool Lexer::match(char expected)
{
    if (isAtEnd() || source_[current_] != expected)
    {
        return false;
    }

    advance();
    return true;
}

void Lexer::scanToken()
{
    const char value = advance();
    switch (value)
    {
    case '(':
        addToken(TokenType::LeftParen);
        break;
    case ')':
        addToken(TokenType::RightParen);
        break;
    case '{':
        addToken(TokenType::LeftBrace);
        break;
    case '}':
        addToken(TokenType::RightBrace);
        break;
    case ':':
        addToken(TokenType::Colon);
        break;
    case ',':
        addToken(TokenType::Comma);
        break;
    case '+':
        addToken(TokenType::Plus);
        break;
    case '-':
        addToken(TokenType::Minus);
        break;
    case '*':
        addToken(TokenType::Star);
        break;
    case '/':
        addToken(TokenType::Slash);
        break;
    case '=':
        addToken(match('=') ? TokenType::EqualEqual : TokenType::Assignment);
        break;
    case '!':
        if (match('='))
        {
            addToken(TokenType::BangEqual);
        }
        else
        {
            reportError("unknown character '!'");
        }
        break;
    case '<':
        addToken(match('=') ? TokenType::LessEqual : TokenType::Less);
        break;
    case '>':
        addToken(match('=') ? TokenType::GreaterEqual : TokenType::Greater);
        break;
    case '"':
        scanString();
        break;
    case ' ':
    case '\r':
    case '\t':
    case '\n':
        break;
    default:
        if (std::isdigit(static_cast<unsigned char>(value)))
        {
            scanNumber();
        }
        else if (isIdentifierStart(value))
        {
            scanIdentifier();
        }
        else
        {
            reportError(std::string("unknown character '") + value + "'");
        }
        break;
    }
}

void Lexer::addToken(TokenType type)
{
    addToken(type, source_.substr(start_, current_ - start_));
}

void Lexer::addToken(TokenType type, const std::string& lexeme)
{
    tokens_.push_back({type, lexeme, tokenLine_, tokenColumn_});
}

void Lexer::scanIdentifier()
{
    while (isIdentifierPart(peek()))
    {
        advance();
    }

    const std::string text = source_.substr(start_, current_ - start_);
    const auto keyword = keywords.find(text);
    addToken(keyword == keywords.end() ? TokenType::Identifier : keyword->second, text);
}

void Lexer::scanNumber()
{
    while (std::isdigit(static_cast<unsigned char>(peek())))
    {
        advance();
    }

    bool isFloat = false;
    if (peek() == '.' && std::isdigit(static_cast<unsigned char>(peekNext())))
    {
        isFloat = true;
        advance();

        while (std::isdigit(static_cast<unsigned char>(peek())))
        {
            advance();
        }
    }

    addToken(isFloat ? TokenType::FloatLiteral : TokenType::IntegerLiteral);
}

void Lexer::scanString()
{
    std::string value;
    bool escaping = false;

    while (!isAtEnd())
    {
        const char current = advance();
        if (escaping)
        {
            switch (current)
            {
            case 'n':
                value.push_back('\n');
                break;
            case 't':
                value.push_back('\t');
                break;
            case '"':
                value.push_back('"');
                break;
            case '\\':
                value.push_back('\\');
                break;
            default:
                value.push_back(current);
                break;
            }
            escaping = false;
            continue;
        }

        if (current == '\\')
        {
            escaping = true;
            continue;
        }

        if (current == '"')
        {
            addToken(TokenType::StringLiteral, value);
            return;
        }

        value.push_back(current);
    }

    reportError("unterminated string");
}

void Lexer::reportError(const std::string& message)
{
    hadError_ = true;
    std::cerr << "MopCode Error: " << message
              << " at line " << tokenLine_
              << ", column " << tokenColumn_ << '\n';
}
}
