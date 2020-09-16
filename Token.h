#ifndef TOKEN_H
#define TOKEN_H

#include "Variant.h"

#include <string>
#include <iostream>
#include <unordered_map>

struct Token {
    enum Type
    {
        KeywordIf,           // if
        KeywordWhile,        // while
        Identifier,          // (any identifier)
        SymbolEquals,        // =
        SymbolAssign,        // :=
        SymbolPlus,          // +
        SymbolMinus,         // -
        StringLiteral,       // std::string
        NumberLiteral,       // double
        SymbolOpeningParens, // (
        SymbolClosingParens, // )
        KeywordTrue,         // true
        KeywordFalse,        // false
        SymbolOpeningCurly,  // {
        SymbolClosingCurly,  // }
        SymbolSemicolon,     // ;
        KeywordAnd,          // and
        KeywordOr,           // or
        Invalid,             // an invalid type
        SymbolComma,         // ,
    } type;
    Variant value;

    std::string print() const {
        switch (type) {
        case Token::SymbolComma:
            return ",";
            break;
        case Token::Invalid:
            return "INVALID";
            break;
        case Token::KeywordAnd:
            return "and";
            break;
        case Token::KeywordOr:
            return "or";
            break;
        case Token::SymbolSemicolon:
            return ";";
            break;
        case Token::SymbolOpeningCurly:
            return "{";
            break;
        case Token::SymbolClosingCurly:
            return "}";
            break;
        case Token::KeywordTrue:
            return "true";
            break;
        case Token::KeywordFalse:
            return "false";
            break;
        case Token::SymbolOpeningParens:
            return "(";
            break;
        case Token::SymbolClosingParens:
            return ")";
            break;
        case Token::KeywordIf:
            return "if";
            break;
        case Token::KeywordWhile:
            return "while";
            break;
        case Token::Identifier:
            return "id(" + value.as<std::string>() + ")";
            break;
        case Token::SymbolEquals:
            return "=";
            break;
        case Token::SymbolAssign:
            return ":=";
            break;
        case Token::SymbolPlus:
            return "+";
            break;
        case Token::SymbolMinus:
            return "-";
            break;
        case Token::StringLiteral:
            return "str(" + value.as<std::string>() + ")";
            break;
        case Token::NumberLiteral:
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
    case Token::SymbolComma:
        return "',' symbol";
        break;
    case Token::Invalid:
        return "INVALID";
        break;
    case Token::KeywordAnd:
        return "'and' keyword";
        break;
    case Token::KeywordOr:
        return "'or' keyword";
        break;
    case Token::KeywordIf:
        return "'if' keyword";
        break;
    case Token::KeywordWhile:
        return "'while' keyword";
        break;
    case Token::Identifier:
        return "identifier";
        break;
    case Token::SymbolEquals:
        return "'=' symbol";
        break;
    case Token::SymbolAssign:
        return "':=' symbol";
        break;
    case Token::SymbolPlus:
        return "'+' symbol";
        break;
    case Token::SymbolMinus:
        return "'-' symbol";
        break;
    case Token::StringLiteral:
        return "string literal";
        break;
    case Token::NumberLiteral:
        return "number literal";
        break;
    case Token::SymbolOpeningParens:
        return "'(' symbol";
        break;
    case Token::SymbolClosingParens:
        return "')' symbol";
        break;
    case Token::KeywordTrue:
        return "'true' keyword";
        break;
    case Token::KeywordFalse:
        return "'false' keyword";
        break;
    case Token::SymbolOpeningCurly:
        return "'{' symbol";
        break;
    case Token::SymbolClosingCurly:
        return "'}' symbol";
        break;
    case Token::SymbolSemicolon:
        return "';' symbol";
        break;
    }
}

static const std::unordered_map<std::string, Token::Type> s_str_token_map = {
    { "if", Token::Type::KeywordIf },
    { "while", Token::Type::KeywordWhile },
    { "=", Token::Type::SymbolEquals },
    { ":=", Token::Type::SymbolAssign },
    { "+", Token::Type::SymbolPlus },
    { "-", Token::Type::SymbolMinus },
    { "(", Token::Type::SymbolOpeningParens },
    { ")", Token::Type::SymbolClosingParens },
    { "true", Token::Type::KeywordTrue },
    { "false", Token::Type::KeywordFalse },
    { "{", Token::Type::SymbolOpeningCurly },
    { "}", Token::Type::SymbolClosingCurly },
    { ";", Token::Type::SymbolSemicolon },
    { "and", Token::Type::KeywordAnd },
    { "or", Token::Type::KeywordOr },
    { ",", Token::Type::SymbolComma },
};

#endif // TOKEN_H
