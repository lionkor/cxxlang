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

    Token::Type peek() const;

    void print_stack() const;

public:
    Interpreter(const std::vector<Token*>& tokens);

    // TODO:
    Result run();
};

#endif // INTERPRETER_H
