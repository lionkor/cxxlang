#include <iostream>
#include "Interpreter.h"
#include "Parser.h"

const std::string src =
    R"(
Number my_number := 4.65;
if (true) {
    String str := "Hello, World! My number is: ";
}
print(string_concat(str, number_to_string(my_number)));
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
