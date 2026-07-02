#pragma once

#include <string>

namespace mopcode
{
enum class TokenType
{
    Function,
    Return,
    If,
    Else,
    For,
    True,
    False,
    Int,
    Float,
    String,
    Bool,
    Void,
    Identifier,
    IntegerLiteral,
    FloatLiteral,
    StringLiteral,
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    Colon,
    Comma,
    Assignment,
    Plus,
    Minus,
    Star,
    Slash,
    Less,
    Greater,
    LessEqual,
    GreaterEqual,
    EqualEqual,
    BangEqual,
    EndOfFile
};

struct Token
{
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};

inline std::string tokenTypeName(TokenType type)
{
    switch (type)
    {
    case TokenType::Function:
        return "FUNCTION";
    case TokenType::Return:
        return "RETURN";
    case TokenType::If:
        return "IF";
    case TokenType::Else:
        return "ELSE";
    case TokenType::For:
        return "FOR";
    case TokenType::True:
        return "TRUE";
    case TokenType::False:
        return "FALSE";
    case TokenType::Int:
        return "INT";
    case TokenType::Float:
        return "FLOAT";
    case TokenType::String:
        return "STRING_TYPE";
    case TokenType::Bool:
        return "BOOL";
    case TokenType::Void:
        return "VOID";
    case TokenType::Identifier:
        return "IDENTIFIER";
    case TokenType::IntegerLiteral:
        return "INTEGER";
    case TokenType::FloatLiteral:
        return "FLOAT_NUMBER";
    case TokenType::StringLiteral:
        return "STRING";
    case TokenType::LeftParen:
        return "LEFT_PAREN";
    case TokenType::RightParen:
        return "RIGHT_PAREN";
    case TokenType::LeftBrace:
        return "LEFT_BRACE";
    case TokenType::RightBrace:
        return "RIGHT_BRACE";
    case TokenType::Colon:
        return "COLON";
    case TokenType::Comma:
        return "COMMA";
    case TokenType::Assignment:
        return "ASSIGNMENT";
    case TokenType::Plus:
        return "PLUS";
    case TokenType::Minus:
        return "MINUS";
    case TokenType::Star:
        return "STAR";
    case TokenType::Slash:
        return "SLASH";
    case TokenType::Less:
        return "LESS";
    case TokenType::Greater:
        return "GREATER";
    case TokenType::LessEqual:
        return "LESS_EQUAL";
    case TokenType::GreaterEqual:
        return "GREATER_EQUAL";
    case TokenType::EqualEqual:
        return "EQUAL_EQUAL";
    case TokenType::BangEqual:
        return "BANG_EQUAL";
    case TokenType::EndOfFile:
        return "EOF";
    }

    return "UNKNOWN";
}
}
