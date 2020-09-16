#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>
#include <stack>
#include <string>
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

    
    void setw() const;
    void handle();
    void consume(Token::Type type);
    Token::Type peek() const;

    void print_stack() const;

public:
    Interpreter(const std::vector<Token*>& tokens);

    // TODO:
    Result run();
};

#endif // INTERPRETER_H
