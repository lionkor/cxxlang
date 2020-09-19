#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>
#include <stack>
#include <map>
#include <string>
#include <functional>
#include <sstream>

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
};

#define error(str) std::cout << "→ Error: " << str

class Interpreter
{
private:
    struct Function {
        std::function<void()> fn;
        size_t argc;
    };

    struct Variable {
        Variant value;
        size_t scope;
    };

    std::stack<Variant> m_stack;
    const std::vector<Token*>& m_tokens;
    bool m_ok { true };
    size_t m_index { 0 };
    static constexpr size_t GLOBAL_SCOPE = 0;
    size_t m_scope_depth { GLOBAL_SCOPE };
    std::map<std::string, Function> m_function_map;
    std::map<std::string, Type> m_type_map;
    std::map<std::string, Variable> m_variables;
    void handle_condition();
    void handle_block();
    void handle_expression();
    void handle_string_expression();
    void handle_numeric_expression();
    void handle_variable_declaration(Type t);
    void skip_block();
    void handle_statement();
    void handle();
    void expect_token(Token::Type type);
    void consume(Token::Type type);
    void consume_blindly();
    void clean_scope();
    const Token& get_token();
    [[nodiscard]] size_t gather_args();

    template<typename... Types>
    std::optional<std::array<Variant, sizeof...(Types)>> stack_expect(Types&&... types) {
        if (stack_size() < sizeof...(Types)) {
            error("couldn't find enough values on the stack to check for valid types" << std::endl);
            m_ok = false;
            return std::nullopt;
        }
        std::array<Variant, sizeof...(Types)> values;
        size_t i = values.size() - 1;
        std::array<Type, sizeof...(Types)> args { types... };
        std::reverse(args.begin(), args.end());
        for (Type type : args) {
            if (stack_top().type() == type) {
                Variant variant = stack_top();
                stack_pop();
                values[i] = variant;
            } else {
                std::stringstream ss;
                ss << "function got unexpected arguments. expected arguments were: \n\t";
                ((ss << type_to_string(types) << " "), ...);
                ss << ". \ninstead got: \n\t";
                auto stack_copy = m_stack;
                for (size_t i = 0; i < sizeof...(Types); ++i) {
                    ss << type_to_string(stack_copy.top().type()) << " ";
                    stack_copy.pop();
                }
                ss << "." << std::endl;
                error(ss.str());
                m_ok = false;
                return std::nullopt;
            }
            i--;
        }
        return values;
    }

    template<typename T>
    void stack_push(T value, Type type) {
        auto variant = Variant(value, type);
        print_stack("before push(" + variant.print() + ")");
        m_stack.push(variant);
        print_stack("after push(" + variant.print() + ")");
    }

    void stack_push(Variant variant) {
        print_stack("before push(" + variant.print() + ")");
        m_stack.push(variant);
        print_stack("after push(" + variant.print() + ")");
    }

    void stack_pop() {
        print_stack("before pop");
        m_stack.pop();
        print_stack("after pop");
    }

    bool stack_empty() {
        return m_stack.empty();
    }

    Variant& stack_top() {
        return m_stack.top();
    }

    size_t stack_size() {
        return m_stack.size();
    }

    const Variant& stack_top() const {
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
