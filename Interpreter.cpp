#include "Interpreter.h"
#include <iomanip>

void Interpreter::setw() const {
    std::cout << std::setfill('.') << std::setw((m_depth - 1) * 4) << "";
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
        break;
    case Token::KeywordOr:
        break;
    case Token::KeywordIf: {
        consume(Token::SymbolOpeningParens);
        handle_condition();
        consume(Token::SymbolClosingParens);
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
    print_stack();
    --m_depth;
}

void Interpreter::expect(Token::Type type) {
    if (m_tokens.at(m_index)->type != type) {
        m_ok = false;
        m_error = "Expected " + nameof(type) + ", got " + nameof(m_tokens[m_index]->type) + " (\"" + m_tokens[m_index]->print() + "\") instead";
        std::cout << "→ Error: " << m_error << std::endl;
    }
}

void Interpreter::consume(Token::Type type) {
    if (m_tokens.at(m_index)->type != type) {
        expect(type);
    }
    ++m_index;
}

void Interpreter::consume_blindly() {
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
