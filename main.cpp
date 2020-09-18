#include <iostream>
#include "Interpreter.h"
#include "Parser.h"

const std::string src =
    R"(
print(number_to_string(5.42));
)";

int main(int, char**) {
    Parser parser(src);
    Interpreter interp(parser.tokens());
    std::cout << "→ running...\n";
    auto res = interp.run();
    if (res.ok) {
        std::cout << "→ returned OK" << std::endl;
    } else {
        std::cout << "→ returned with Error" << std::endl;
    }
}
