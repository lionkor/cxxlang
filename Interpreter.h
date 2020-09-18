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
    const std::vector<Token*>& m_tokens;
    bool m_ok { true };
    std::string m_error { "" };
    size_t m_index { 0 };
    std::map<std::string, std::function<void()>> m_function_map;

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

    template<typename... Types>
    std::optional<std::array<Variant, sizeof...(Types)>> stack_expect(Types&&... types) {
        std::array<Variant, sizeof...(Types)> values;
        size_t i = values.size() - 1;
        std::array<Type, sizeof...(Types)> args { types... };
        std::reverse(args.begin(), args.end());
        for (Type type : args) {
            if (!m_stack.empty() && top().type() == type) {
                Variant variant = top();
                pop();
                values[i] = variant;
            } else {
                std::cout << "→ Error: function got unexpected arguments. expected arguments were: ";
                ((std::cout << type_to_string(types) << " "), ...);
                std::cout << std::endl;
                return std::nullopt;
            }
            i--;
        }
        return values;
    }

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
