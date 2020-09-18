#include <iostream>
#include "Interpreter.h"
#include "Parser.h"

const std::string src =
    R"(
if (true and (false or (true and (false or true)))) {
    print("Hello, World!");
}
)";

int main(int, char**) {
    Parser parser(src);
    Interpreter interp(parser.tokens());
    std::cout << "→ running...\n";
    auto res = interp.run();
    if (res.ok) {
        std::cout << "→ returned OK" << std::endl;
    } else {
        std::cout << "→ returned with Error: " << res.message << std::endl;
    }
}
