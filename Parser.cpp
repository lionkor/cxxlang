#include "Parser.h"

#include <algorithm>
#include <map>
#include <iomanip>

#include "Token.h"

Parser::Parser(const std::string& src) {
    //auto search_for = [&](auto start, auto str) { return std::search(start, src.end(), str.begin(), str.end()); };

    for (size_t i = 0; i < src.size();) {
        bool found = false;
        for (const auto& pair : s_str_token_map) {
            auto substr = src.substr(i, pair.first.size());
            if (substr == pair.first) {
                Token* tok = new Token { pair.second, Variant(substr, Type::String) };
                std::cout << "→ Found token: " << tok->print() << std::endl;
                m_tokens.push_back(tok);
                i += substr.size();
                found = true;
                break;
            } else {
                auto tok = Token { pair.second, Variant(substr, Type::String) };
                //std::cout << "  ( _" << substr << "_ is not " << tok.print() << ")" << std::endl;
            }
        }

        // handle numbers
        if (!found && std::isdigit(src[i])) {
            std::string number_str;
            // FIXME : this is shit
            number_str.resize(15, ' ');
            auto end = std::copy_if(src.begin() + i, src.end(), number_str.begin(),
                [&](char c) -> bool {
                    return std::isdigit(c)
                           || c == '.';
                });
            auto tok = new Token { Token::Type::NumberLiteral, Variant(std::stod(number_str), Type::Number) };
            std::cout << "→ Found token: " << tok->print() << std::endl;
            m_tokens.push_back(tok);
            // advance i
            i += end - number_str.begin();
            found = true;
        }

        // consume whitespace
        if (!found && std::isspace(src[i])) {
            ++i;
        } else if (!found) {
            // is not whitespace and nothing was found here
            std::cout << "→ Error: Unknown symbol at position " << i << ": \n\t" << src.substr(i, 5) << "\n"
                      << "\t^\n";
            break;
        }
    }
    std::cout << "→ done parsing!" << std::endl;
}
