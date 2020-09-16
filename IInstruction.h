#ifndef IINSTRUCTION_H
#define IINSTRUCTION_H

// interface
class IInstruction
{
public:
    virtual void execute(std::stack<Variant>& stack) = 0;
};

#endif // IINSTRUCTION_H
