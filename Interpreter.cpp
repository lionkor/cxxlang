#include "Interpreter.h"

Interpreter::Interpreter(std::vector<Token*>&& tokens)
    : m_tokens(tokens) { }

Result Interpreter::run() {
    for (const Token* token : m_tokens) {
    }
    return { true, "" };
}
