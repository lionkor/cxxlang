#include <iostream>
#include "Interpreter.h"
#include "Parser.h"

int main(int, char**) {
    //Parser parser("if (true or false) { print(\"hello\"); )}");
    //Parser parser("if (false or (true or false))");
    Parser parser("if (true) { if (false) { print(\"hi\"); } if (true) { print(\"ok\"); } }");
    Interpreter interp(parser.tokens());
    std::cout << "→ running...\n";
    auto res = interp.run();
    if (res.ok) {
        std::cout << "→ returned OK" << std::endl;
    } else {
        std::cout << "→ returned with Error: " << res.message << std::endl;
    }
}
