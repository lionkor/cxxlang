#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>
#include <stack>
#include <map>
#include <string>
#include <functional>

#include "Variant.h"
#include "Token.h"

enum Context
{
    Condition,
    Statement,
    Block,
};

struct Result {
    const bool ok { true };
    const std::string message { "" };
};

class Interpreter
{
private:
    std::stack<Variant> m_stack;
    std::stack<Context> m_context;
    const std::vector<Token*>& m_tokens;
    bool m_ok { true };
    std::string m_error { "" };
    size_t m_index { 0 };
    size_t m_depth;
    std::map<std::string, std::function<void()>> m_function_map;

    void setw() const;
    void handle_condition();
    void handle_block();
    void handle_expression();
    void handle_string_expression();
    void handle_numeric_expression();
    void skip_block();
    void handle_statement();
    void handle();
    void expect(Token::Type type);
    void consume(Token::Type type);
    void consume_blindly();
    void gather_args();

    template<typename T>
    void push(T value, Type type) {
        auto variant = Variant(value, type);
        print_stack("before push(" + variant.print() + ")");
        m_stack.push(variant);
        print_stack("after push(" + variant.print() + ")");
    }

    void push(Variant variant) {
        print_stack("before push(" + variant.print() + ")");
        m_stack.push(variant);
        print_stack("after push(" + variant.print() + ")");
    }

    void pop() {
        print_stack("before pop");
        m_stack.pop();
        print_stack("after pop");
    }

    Variant& top() {
        return m_stack.top();
    }

    const Variant& top() const {
        return m_stack.top();
    }

    Token::Type peek() const;

    void print_stack(const std::string& where) const;

public:
    Interpreter(const std::vector<Token*>& tokens);

    // TODO:
    Result run();
};

#endif // INTERPRETER_H
