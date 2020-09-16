#include "Interpreter.h"
#include <iomanip>

void Interpreter::setw() const {
    std::cout << std::setfill('.') << std::setw((m_depth - 1)) << "";
}

void Interpreter::handle_condition() {
    // pre stack: ?
    // post stack: bool containing the resulting value

    ++m_depth;

    print_stack();
    if (peek() == Token::SymbolOpeningParens) {
        consume(Token::SymbolOpeningParens);
        handle_condition();
        consume(Token::SymbolClosingParens);
    } else {
        print_stack();

        if (peek() == Token::KeywordTrue) {
            consume(Token::KeywordTrue);
            m_stack.push(Variant(true, Type::Bool));
        } else if (peek() == Token::KeywordFalse) {
            consume(Token::KeywordFalse);
            m_stack.push(Variant(false, Type::Bool));
        } else if (peek() == Token::Identifier) {
            std::cout << "identifiers not yet supported in conditions, defaulting to false" << std::endl;
            consume(Token::Identifier);
            m_stack.push(Variant(false, Type::Bool));
        } else {
            std::cout << "→ Error: invalid condition, unexpected token: " << nameof(peek()) << " at index " << m_index << std::endl;
            m_ok = false;
            return;
        }
    }
    print_stack();

    if (peek() == Token::KeywordAnd || peek() == Token::KeywordOr) {
        auto type = peek();
        consume_blindly();
        handle_condition();
        bool second = m_stack.top().as<bool>();
        m_stack.pop();
        bool first = m_stack.top().as<bool>();
        m_stack.pop();
        if (type == Token::KeywordAnd) {
            m_stack.push(Variant(first && second, Type::Bool));
        } else {
            m_stack.push(Variant(first || second, Type::Bool));
        }
    }
    --m_depth;
    print_stack();
}

void Interpreter::handle_block() {
    ++m_depth;
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
            handle();
        }
    }
    consume(Token::SymbolClosingCurly);
    --m_depth;
}

void Interpreter::handle_expression() {
    // pre stack: ?
    // post stack: the value(s) of the expression
    if (peek() == Token::Type::StringLiteral) {
        handle_string_expression();
    } else if (peek() == Token::Type::NumberLiteral) {
        handle_numeric_expression();
    } else {
        std::cout << "→ Error: Unknown expression kind: " << nameof(peek()) << std::endl;
    }
}

void Interpreter::handle_string_expression() {
    auto tok = m_tokens.at(m_index);
    consume(Token::Type::StringLiteral);
    m_stack.push(tok->value);
}

void Interpreter::handle_numeric_expression() {
    auto tok = m_tokens.at(m_index);
    consume(Token::Type::NumberLiteral);
    m_stack.push(tok->value);
    // TODO: Call Calculator for this
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
}

void Interpreter::handle() {
    // pre stack: ?
    // post stack: ?

    ++m_depth;
    print_stack();
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
        bool condition = m_stack.top().as<bool>();
        m_stack.pop();
        if (condition == true) {
            handle_block();
        } else {
            skip_block();
        }
        break;
    }
    case Token::KeywordWhile:
        break;
    case Token::Identifier: {
        auto val = tok.value.as<std::string>();
        if (m_function_map.find(val) != m_function_map.end()) {
            // get args
            gather_args();
            m_function_map[val]();
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
        break;
    case Token::NumberLiteral:
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
    print_stack();
    --m_depth;
}

void Interpreter::expect(Token::Type type) {
    if (m_index >= m_tokens.size()) {
        m_ok = false;
        m_error = "Expected " + nameof(type) + ", got end of code instead, at index " + std::to_string(m_index);
        std::cout << "→ Error: " << m_error << std::endl;
    } else if (m_tokens.at(m_index)->type != type) {
        m_ok = false;
        m_error = "Expected " + nameof(type) + ", got " + nameof(m_tokens[m_index]->type) + " (\"" + m_tokens[m_index]->print() + "\") instead, at index " + std::to_string(m_index);
        std::cout << "→ Error: " << m_error << std::endl;
    }
}

void Interpreter::consume(Token::Type type) {
    if (m_index >= m_tokens.size() || m_tokens.at(m_index)->type != type) {
        expect(type);
    }
    ++m_index;
}

void Interpreter::consume_blindly() {
    ++m_index;
}

void Interpreter::gather_args() {
    consume(Token::SymbolOpeningParens);
    size_t parens = 1;
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
            if (peek() != Token::SymbolComma) {
                break;
            }
        }
    }
    consume(Token::SymbolClosingParens);
}

Token::Type Interpreter::peek() const {
    if (m_index >= m_tokens.size()) {
        return Token::Type::Invalid;
    }
    return m_tokens.at(m_index)->type;
}

void Interpreter::print_stack() const {
#if 0
    setw();
    std::cout << "stack: \n";
    std::stack stack_copy = m_stack;
    while (!stack_copy.empty()) {
        setw();
        std::cout << " > " << stack_copy.top().print() << "\n";
        stack_copy.pop();
    }
    std::cout << std::endl;
#endif
}

Interpreter::Interpreter(const std::vector<Token*>& tokens)
    : m_tokens(tokens) {
    auto fn_print = [&] {
        Variant arg = m_stack.top();
        if (arg.is(Type::String)) {
            std::cout << arg.as<std::string>() << std::endl;
        } else {
            std::cout << "→ Error: invalid argument to print()" << std::endl;
        }
    };

    m_function_map = {
        { "print", fn_print },
    };
}


Result Interpreter::run() {
    print_stack();
    m_index = 0;
    m_depth = 0;
    while (m_ok && m_index < m_tokens.size()) {
        handle();
    }

    print_stack();
    return { m_ok, m_error };
}
