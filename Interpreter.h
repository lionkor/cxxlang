#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>
#include <stack>
#include <map>
#include <string>
#include <functional>
#include <sstream>

#define DBG 0

#include "Variant.h"
#include "Token.h"
#include "Debug.h"

#define stack_pop()                                                                                        \
    do {                                                                                                   \
        /*std::cout << "stack_pop called from: " << __PRETTY_FUNCTION__ << ":" << __LINE__ << std::endl;*/ \
        stack_pop_impl();                                                                                  \
    } while (false)

#define stack_push                                                                                      \
    /*std::cout << "stack_push called from: " << __PRETTY_FUNCTION__ << ":" << __LINE__ << std::endl;*/ \
    stack_push_impl

#if not DBG
#define dbg(x)
#else
#define dbg(x) std::cout << __FUNCTION__ << ":" << __LINE__ << ": " << x << std::endl
#endif

enum Context
{
    Condition,
    Statement,
    Block,
};

struct Result {
    const bool ok { true };
};


#define error(str) std::cout << "→ Error: " << str << std::endl

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
    std::vector<Token*>& m_tokens;
    bool m_ok { true };
    Debug<size_t> m_index { 0 };
    static constexpr size_t GLOBAL_SCOPE = 0;
    size_t m_scope_depth { GLOBAL_SCOPE };
    std::map<std::string, Function> m_function_map;
    std::map<std::string, Type> m_type_map;
    std::map<std::string, Variable> m_variables;
    void handle_condition();
    void handle_block();
    void handle_main();
    void handle_expression();
    void handle_string_expression();
    void handle_numeric_expression();
    void handle_variable_declaration(Type t);
    void skip_block();
    void handle();
    void expect_token(Token::Type type);
    void consume(Token::Type type);
    void consume_blindly();
    void clean_scope();
    size_t call_function(const std::string& val, size_t index);
    const Token& get_token();
    [[nodiscard]] size_t gather_args();
    bool is_numeric_expression_part(Token::Type t);
    void calculate();

    template<typename... Types>
    std::optional<std::array<Variant, sizeof...(Types)>> stack_expect(Types&&... types) {
        // pre cond: ?
        // post cond: stack.size() -= sizeof...(Types)
        size_t pre_stack_size = m_stack.size();
        if (stack_size() < sizeof...(Types)) {
            calculate();
            stack_pop();
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
                ss << "unexpected arguments. expected arguments were: \n\t";
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
        if (m_stack.size() != pre_stack_size - sizeof...(Types)) {
            error("stack_expect didn't pop sizeof...(Types) values: " << m_stack.size() - pre_stack_size);
            m_ok = false;
            return std::nullopt;
        }
        return values;
    }

    template<typename T>
    void stack_push_impl(T value, Type type) {
        auto variant = Variant(value, type);
        print_stack("before push(" + variant.print() + ")");
        m_stack.push(variant);
        print_stack("after push(" + variant.print() + ")");
    }

    void stack_push_impl(Variant variant) {
        print_stack("before push(" + variant.print() + ")");
        m_stack.push(variant);
        print_stack("after push(" + variant.print() + ")");
    }

    void stack_pop_impl() {
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
    Interpreter(std::vector<Token*>& tokens);

    // TODO:
    Result run();
};

#endif // INTERPRETER_H
