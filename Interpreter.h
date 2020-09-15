#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>
#include <stack>
#include "Variant.h"

// interface
class IInstruction
{
public:
    virtual void execute(std::stack<Variant>& stack) = 0;
};

class Interpreter
{
private:
    std::stack<Variant> m_stack;
    std::vector<IInstruction> m_instrs;

public:
    Interpreter(std::vector<IInstruction>&& instrs);

    // TODO:
    Result run();
};

#endif // INTERPRETER_H
