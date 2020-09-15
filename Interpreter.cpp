#include "Interpreter.h"

Interpreter::Interpreter(std::vector<IInstruction>&& instrs)
    : m_instrs(instrs) { }

