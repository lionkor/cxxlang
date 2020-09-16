#include "Interpreter.h"
#include <iomanip>

void Interpreter::setw() const {
    std::cout << std::setfill('.') << std::setw((m_depth - 1) * 4) << " ";
}

void Interpreter::handle() {
    ++m_depth;
    print_stack();
    if (!m_ok) {
        return;
    }
    const Token& tok = *m_tokens.at(m_index);
    ++m_index;
    setw();
    std::cout << "processing " << tok.print() << std::endl;
    switch (tok.type) {
    case Token::KeywordAnd: {
        if (m_context.top() == Context::Condition
            && m_stack.top().type() == Type::Bool) {
            // more conditions!
            handle();
            if (m_stack.top().type() != Type::Bool) {
                std::cout << "→ and followed by non-bool" << std::endl;
            }
            bool second = m_stack.top().as<bool>();
            m_stack.pop();
            bool first = m_stack.top().as<bool>();
            m_stack.pop();
            m_stack.push(Variant(first && second, Type::Bool));
        }
        break;
    }
    case Token::KeywordOr:
        break;
    case Token::KeywordIf: {
        consume(Token::SymbolOpeningParens);
        m_context.push(Context::Condition);
        handle();
        if (m_context.top() == Context::Condition) {
            m_context.pop();
        } else {
            std::cout << "→ Invalid context detected" << std::endl;
        }
        consume(Token::SymbolClosingParens);
        // evaluate stack contents
        if (m_stack.top().type() != Type::Bool) {
            std::cout << "→ Unexpected stack contents" << std::endl;
        }
        bool res = m_stack.top().as<bool>();
        m_stack.pop();
        break;
    }
    case Token::KeywordWhile:
        break;
    case Token::Identifier:
        break;
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
        if (m_context.top() == Context::Condition) {
            m_stack.push(Variant(true, Type::Bool));
            if (peek() == Token::Type::KeywordAnd
                || peek() == Token::Type::KeywordOr) {
                handle();
            }
        }
        break;
    case Token::KeywordFalse:
        if (m_context.top() == Context::Condition) {
            m_stack.push(Variant(false, Type::Bool));
            if (peek() == Token::Type::KeywordAnd
                || peek() == Token::Type::KeywordOr) {
                handle();
            }
        }
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

void Interpreter::consume(Token::Type type) {
    if (m_tokens.at(m_index)->type != type) {
        m_ok = false;
        m_error = "Expected " + nameof(type) + ", got " + nameof(m_tokens[m_index]->type) + " (\"" + m_tokens[m_index]->print() + "\") instead";
        std::cout << "→ Error: " << m_error << std::endl;
    }
    ++m_index;
}

Token::Type Interpreter::peek() const {
    return m_tokens.at(m_index)->type;
}

void Interpreter::print_stack() const {
    setw();
    std::cout << "stack: \n";
    std::stack stack_copy = m_stack;
    while (!stack_copy.empty()) {
        setw();
        std::cout << " > " << stack_copy.top().print() << "\n";
        stack_copy.pop();
    }
    std::cout << std::endl;
}

Interpreter::Interpreter(const std::vector<Token*>& tokens)
    : m_tokens(tokens) { }


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
