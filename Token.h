#ifndef TOKEN_H
#define TOKEN_H

#include "Variant.h"

#include <string>
#include <iostream>
#include <unordered_map>

struct Token {
    enum Type
    {
        If,             // if
        While,          // while
        Identifier,     // (any identifier)
        Equals,         // =
        Assign,         // :=
        Plus,           // +
        Minus,          // -
        Star,           // *
        Slash,          // /
        StringLiteral,  // std::string
        NumericLiteral, // double
        OpeningParens,  // (
        ClosingParens,  // )
        True,           // true
        False,          // false
        OpeningCurly,   // {
        ClosingCurly,   // }
        Semicolon,      // ;
        And,            // and
        Or,             // or
        Comma,          // ,
        Invalid,        // an invalid type
    } type;
    Variant value;

    std::string print() const {
        switch (type) {
        case Token::Star:
            return "*";
            break;
        case Token::Slash:
            return "/";
            break;
        case Token::Comma:
            return ",";
            break;
        case Token::Invalid:
            return "INVALID";
            break;
        case Token::And:
            return "and";
            break;
        case Token::Or:
            return "or";
            break;
        case Token::Semicolon:
            return ";";
            break;
        case Token::OpeningCurly:
            return "{";
            break;
        case Token::ClosingCurly:
            return "}";
            break;
        case Token::True:
            return "true";
            break;
        case Token::False:
            return "false";
            break;
        case Token::OpeningParens:
            return "(";
            break;
        case Token::ClosingParens:
            return ")";
            break;
        case Token::If:
            return "if";
            break;
        case Token::While:
            return "while";
            break;
        case Token::Identifier:
            return "id(" + value.as<std::string>() + ")";
            break;
        case Token::Equals:
            return "=";
            break;
        case Token::Assign:
            return ":=";
            break;
        case Token::Plus:
            return "+";
            break;
        case Token::Minus:
            return "-";
            break;
        case Token::StringLiteral:
            return "str(" + value.as<std::string>() + ")";
            break;
        case Token::NumericLiteral:
            return "num(" + std::to_string(value.as<double>()) + ")";
            break;
        default:
            return "Unknown ???";
            break;
        }
        return "doesn't happen!?"; // doesnt happen
    }
};

static inline std::string nameof(Token::Type t) {
    switch (t) {
    case Token::Star:
        return "'*' symbol";
        break;
    case Token::Slash:
        return "'/' symbol";
        break;
    case Token::Comma:
        return "',' symbol";
        break;
    case Token::Invalid:
        return "INVALID";
        break;
    case Token::And:
        return "'and' keyword";
        break;
    case Token::Or:
        return "'or' keyword";
        break;
    case Token::If:
        return "'if' keyword";
        break;
    case Token::While:
        return "'while' keyword";
        break;
    case Token::Identifier:
        return "identifier";
        break;
    case Token::Equals:
        return "'=' symbol";
        break;
    case Token::Assign:
        return "':=' symbol";
        break;
    case Token::Plus:
        return "'+' symbol";
        break;
    case Token::Minus:
        return "'-' symbol";
        break;
    case Token::StringLiteral:
        return "string literal";
        break;
    case Token::NumericLiteral:
        return "number literal";
        break;
    case Token::OpeningParens:
        return "'(' symbol";
        break;
    case Token::ClosingParens:
        return "')' symbol";
        break;
    case Token::True:
        return "'true' keyword";
        break;
    case Token::False:
        return "'false' keyword";
        break;
    case Token::OpeningCurly:
        return "'{' symbol";
        break;
    case Token::ClosingCurly:
        return "'}' symbol";
        break;
    case Token::Semicolon:
        return "';' symbol";
        break;
    }
}

static const std::unordered_map<std::string, Token::Type> s_str_token_map = {
    { "if", Token::Type::If },
    { "while", Token::Type::While },
    { "=", Token::Type::Equals },
    { ":=", Token::Type::Assign },
    { "+", Token::Type::Plus },
    { "-", Token::Type::Minus },
    { "*", Token::Type::Star },
    { "/", Token::Type::Slash },
    { "(", Token::Type::OpeningParens },
    { ")", Token::Type::ClosingParens },
    { "true", Token::Type::True },
    { "false", Token::Type::False },
    { "{", Token::Type::OpeningCurly },
    { "}", Token::Type::ClosingCurly },
    { ";", Token::Type::Semicolon },
    { "and", Token::Type::And },
    { "or", Token::Type::Or },
    { ",", Token::Type::Comma },
};

#endif // TOKEN_H
