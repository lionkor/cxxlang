#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>
#include <stack>
#include <string>
#include "Variant.h"
#include "Token.h"

struct Result {
    const bool ok { true };
    const std::string message { "" };
};

class Interpreter
{
private:
    std::stack<Variant> m_stack;
    std::vector<Token*> m_tokens;

public:
    Interpreter(std::vector<Token*>&& tokens);

    // TODO:
    Result run();
};

#endif // INTERPRETER_H
