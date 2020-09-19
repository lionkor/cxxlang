#include "Interpreter.h"
#include <iomanip>
#include <charconv>

void Interpreter::handle_condition() {
    // pre stack: ?
    // post stack: bool containing the resulting value


    // TODO: abstract away stack operations, then call print_stack on each operation :)


    if (peek() == Token::SymbolOpeningParens) {
        consume(Token::SymbolOpeningParens);
        handle_condition();
        consume(Token::SymbolClosingParens);
    } else {
        if (peek() == Token::KeywordTrue) {
            consume(Token::KeywordTrue);
            stack_push(true, Type::Bool);
        } else if (peek() == Token::KeywordFalse) {
            consume(Token::KeywordFalse);
            stack_push(false, Type::Bool);
        } else if (peek() == Token::Identifier) {
            std::cout << "identifiers not yet supported in conditions, defaulting to false" << std::endl;
            consume(Token::Identifier);
            stack_push(false, Type::Bool);
        } else {
            error("invalid condition, unexpected token: " << nameof(peek()) << " at index " << m_index << std::endl);
            m_ok = false;
            return;
        }
    }

    if (peek() == Token::KeywordAnd || peek() == Token::KeywordOr) {
        auto type = peek();
        consume_blindly();
        handle_condition();
        bool second = stack_top().as<bool>();
        stack_pop();
        bool first = stack_top().as<bool>();
        stack_pop();
        if (type == Token::KeywordAnd) {
            stack_push(first && second, Type::Bool);
        } else {
            stack_push(first || second, Type::Bool);
        }
    }
}

void Interpreter::handle_block() {
    consume(Token::SymbolOpeningCurly);
    size_t curlys = 1;
    while (m_index < m_tokens.size()) {
        if (peek() == Token::SymbolOpeningCurly) {
            curlys += 1;
        } else if (peek() == Token::SymbolClosingCurly) {
            curlys -= 1;
        }
        if (curlys == 0) {
            break;
        } else {
            handle_statement();
        }
    }
    consume(Token::SymbolClosingCurly);
}

void Interpreter::handle_expression() {
    // pre stack: ?
    // post stack: the value(s) of the expression
    if (peek() == Token::Type::StringLiteral) {
        handle_string_expression();
    } else if (peek() == Token::Type::NumberLiteral) {
        handle_numeric_expression();
    } else if (peek() == Token::Type::Identifier) {
        // function call
        handle();
    } else {
        error("Unknown expression kind: " << nameof(peek()) << std::endl);
    }
}

void Interpreter::handle_string_expression() {
    auto tok = m_tokens.at(m_index);
    consume(Token::Type::StringLiteral);
    stack_push(tok->value);
}

void Interpreter::handle_numeric_expression() {
    auto tok = m_tokens.at(m_index);
    consume(Token::Type::NumberLiteral);
    stack_push(tok->value);
    // TODO: Call Calculator for this
}

void Interpreter::handle_variable_declaration(Type t) {
    // get variable ID
    auto name = get_token();
    if (name.type != Token::Identifier) {
        error("expected identifier after typename" << std::endl);
        m_ok = false;
        return;
    }
    consume(Token::Identifier);
    auto name_literal = name.value.as<std::string>();
    consume(Token::SymbolAssign);
    handle_expression();
    consume(Token::SymbolSemicolon);
    // now we should have a value on the stack!
    if (stack_empty()) {
        error("cannot assign void" << std::endl);
        m_ok = false;
        return;
    }
    // and there should be no variable with the same name
    if (m_variables.find(name_literal) != m_variables.end()) {
        error("cannot declare variable - variable with same name already exists!" << std::endl);
        m_ok = false;
        return;
    }
    // all ok, now store
    m_variables[name_literal] = stack_top();
    stack_pop();
}

void Interpreter::skip_block() {
    consume(Token::SymbolOpeningCurly);
    size_t curlys = 1;
    for (;;) {
        if (peek() == Token::SymbolOpeningCurly) {
            curlys += 1;
        } else if (peek() == Token::SymbolClosingCurly) {
            curlys -= 1;
        }
        if (curlys == 0) {
            break;
        } else {
            // do nothing, as we just skip!
            consume_blindly();
        }
    }
    consume(Token::SymbolClosingCurly);
}

void Interpreter::handle_statement() {
    handle();
    consume(Token::SymbolSemicolon);
}

void Interpreter::handle() {
    // pre stack: ?
    // post stack: ?

    if (!m_ok) {
        return;
    }
    const Token& tok = *m_tokens.at(m_index);
    ++m_index;
#if 0
    setw();
    std::cout << "processing " << tok.print() << std::endl;
#endif
    switch (tok.type) {
    case Token::SymbolComma:
        break;
    case Token::Invalid:
        break;
    case Token::KeywordAnd:
        break;
    case Token::KeywordOr:
        break;
    case Token::KeywordIf: {
        consume(Token::SymbolOpeningParens);
        handle_condition();
        consume(Token::SymbolClosingParens);
        bool condition = stack_top().as<bool>();
        stack_pop();
        if (condition == true) {
            handle_block();
        } else {
            skip_block();
        }
        break;
    }
    case Token::KeywordWhile: {
        consume(Token::SymbolOpeningParens);
        handle_condition();
        consume(Token::SymbolClosingParens);
        bool condition = stack_top().as<bool>();
        stack_pop();
        size_t start_of_block = m_index;
        std::optional<size_t> end_of_block = std::nullopt;
        while (condition == true) {
            handle_block();
            end_of_block = m_index;
            m_index = start_of_block;
        }
        if (end_of_block.has_value()) {
            m_index = end_of_block.value();
        } else {
            skip_block();
        }
        //skip_block();
        break;
    }
    case Token::Identifier: {
        auto val = tok.value.as<std::string>();
        if (m_function_map.find(val) != m_function_map.end()) {
            // function call!
            // get args
            size_t argc = gather_args();
            if (argc != m_function_map[val].argc) {
                error("\"" << val << "\" expects " << m_function_map[val].argc << " arguments, got " << argc << " instead." << std::endl);
                m_ok = false;
                return;
            } else {
                m_function_map[val].fn();
            }
        } else if (m_type_map.find(val) != m_type_map.end()) {
            // variable declaration!
            handle_variable_declaration(m_type_map[val]);
        } else if (m_variables.find(val) != m_variables.end()) {
            stack_push(m_variables[val]);
        } else {
            error("identifier \"" << val << "\" doesn't map to any function or value" << std::endl);
            m_ok = false;
            return;
        }
        break;
    }
    case Token::SymbolEquals:
        break;
    case Token::SymbolAssign:
        break;
    case Token::SymbolPlus:
        break;
    case Token::SymbolMinus:
        break;
    case Token::StringLiteral:
        // handle_string_expression();
        break;
    case Token::NumberLiteral:
        // handle_numeric_expression();
        break;
    case Token::SymbolOpeningParens:
        break;
    case Token::SymbolClosingParens:
        break;
    case Token::KeywordTrue:
        break;
    case Token::KeywordFalse:
        break;
    case Token::SymbolOpeningCurly:
        break;
    case Token::SymbolClosingCurly:
        break;
    case Token::SymbolSemicolon:
        break;
    }
}

void Interpreter::expect_token(Token::Type type) {
    if (m_index >= m_tokens.size()) {
        error("Expected " + nameof(type) + ", got end of code instead, at index " + std::to_string(m_index) << std::endl);
        m_ok = false;
    } else if (m_tokens.at(m_index)->type != type) {
        error("Expected " + nameof(type) + ", got " + nameof(m_tokens[m_index]->type) + " (\"" + m_tokens[m_index]->print() + "\") instead, at index " + std::to_string(m_index) << std::endl);
        m_ok = false;
    }
}

void Interpreter::consume(Token::Type type) {
    if (m_index >= m_tokens.size() || m_tokens.at(m_index)->type != type) {
        expect_token(type);
    }
    ++m_index;
}

void Interpreter::consume_blindly() {
    ++m_index;
}

const Token& Interpreter::get_token() {
    return *m_tokens[m_index];
}

[[nodiscard]] size_t Interpreter::gather_args() {
    consume(Token::SymbolOpeningParens);
    size_t parens = 1;
    size_t args = 0;
    for (;;) {
        if (peek() == Token::SymbolOpeningParens) {
            parens += 1;
        } else if (peek() == Token::SymbolClosingParens) {
            parens -= 1;
        }
        if (parens == 0) {
            break;
        } else {
            handle_expression();
            ++args;
            if (peek() != Token::SymbolComma) {
                break;
            } else {
                std::cout << "got comma\n";
                consume(Token::SymbolComma);
            }
        }
    }
    consume(Token::SymbolClosingParens);
    return args;
}


Token::Type Interpreter::peek() const {
    if (m_index >= m_tokens.size()) {
        return Token::Type::Invalid;
    }
    return m_tokens.at(m_index)->type;
}

void Interpreter::print_stack(const std::string& where) const {
#if 0
    std::cout << " > stack " << where << "\n";
    std::stack stack_copy = m_stack;
    size_t i = 0;
    while (!stack_copy.empty()) {
        std::cout << "    ";
        if (i == 0) {
            std::cout << "top: ";
        } else {
            std::cout << std::setw(3) << i << ": ";
        }
        std::cout << stack_copy.top().print() << "\n";
        stack_copy.pop();
        ++i;
    }
    std::cout << std::endl;
#endif
}

Interpreter::Interpreter(const std::vector<Token*>& tokens)
    : m_tokens(tokens) {
    auto fn_print = [&] {
        auto res = stack_expect(Type::String);
        if (res.has_value()) {
            auto [str] = res.value();
            std::cout << "" << str.as<std::string>() << std::endl;
        } else {
            error("call to print failed, invalid args" << std::endl);
            m_ok = false;
            return;
        }
    };

    auto fn_string_concat = [&] {
        auto res = stack_expect(Type::String, Type::String);
        if (res.has_value()) {
            auto [str1, str2] = res.value();
            stack_push(str1.as<std::string>() + str2.as<std::string>(), Type::String);
        } else {
            error("call to string_concat failed, invalid args" << std::endl);
            m_ok = false;
            return;
        }
    };

    auto fn_number_to_string = [&] {
        auto res = stack_expect(Type::Number);
        if (res.has_value()) {
            auto [num] = res.value();
            std::string str;
            str.resize(30);
            auto n = std::sprintf(str.data(), "%f", num.as<double>());
            stack_push(str.substr(0, n), Type::String);
        } else {
            error("call to number_to_string failed, invalid args" << std::endl);
            m_ok = false;
            return;
        }
    };

    // map of name to (function, arg_count)
    m_function_map = {
        { "print", { fn_print, 1 } },
        { "string_concat", { fn_string_concat, 2 } },
        { "number_to_string", { fn_number_to_string, 1 } },
    };

    m_type_map = {
        { "Number", Type::Number },
        { "String", Type::String },
        { "Array", Type::Array },
        { "Bool", Type::Bool },
    };

    m_variables = {
        { "PI", Variant(double(3.1415926535897932384626433832795028841), Type::Number) },
    };
}


Result Interpreter::run() {
    m_index = 0;
    while (m_ok && m_index < m_tokens.size()) {
        handle();
    }

    print_stack("at end of program");
    return { m_ok };
}
