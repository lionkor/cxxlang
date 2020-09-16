#include <iostream>
#include "Interpreter.h"
#include "Parser.h"

int main(int, char**) {
    Parser parser("if (true and false) { print(\"hello\"); )}");
    Interpreter interp(parser.tokens());
    std::cout << "→ running...\n";
    auto res = interp.run();
    if (res.ok) {
        std::cout << "→ returned OK" << std::endl;
    } else {
        std::cout << "→ returned with Error: " << res.message << std::endl;
    }
}
