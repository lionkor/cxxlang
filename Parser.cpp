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
                //auto tok = Token { pair.second, Variant(substr, Type::String) };
                //std::cout << "  ( _" << substr << "_ is not " << tok.print() << ")" << std::endl;
            }
        }

        // handle number literals
        if (!found && std::isdigit(src[i])) {
            // FIXME : this is shit
            bool found_dot = false;
            auto end = std::find_if_not(src.begin() + i, src.end(),
                [&](char c) -> bool {
                    if (!found_dot && c == '.') {
                        found_dot = true;
                        return true;
                    } else {
                        return std::isdigit(c);
                    }
                });
            auto number_str = std::string(src.begin() + i, end);
            auto tok = new Token { Token::Type::NumericLiteral, Variant(std::stod(number_str), Type::Number) };
            std::cout << "→ Found token: " << tok->print() << std::endl;
            m_tokens.push_back(tok); 
            // advance i
            i += number_str.size();
            found = true;
        }

        // handle string literals
        if (!found && src[i] == '"') {
            std::string str;
            size_t k = i + 1;
            for (;; ++k) {
                if (src[k] == '"') {
                    auto tok = new Token { Token::Type::StringLiteral, Variant(str, Type::String) };
                    std::cout << "→ Found token: " << tok->print() << std::endl;
                    m_tokens.push_back(tok);
                    i = k + 1;
                    found = true;
                    break;
                } else if (k >= src.size()) {
                    // ERROR
                    std::cout << "→ Error: Unterminated string literal starting at " << i << std::endl;
                    break;
                }
                str += src[k];
            }
        }

        auto is_id_char = [](char c) -> bool {
            return std::isalnum(c) || c == '_';
        };

        // handle identifiers
        if (!found && (std::isalpha(src[i]) || src[i] == '_')) {
            std::string id;
            size_t k = i;
            for (; k < src.size(); ++k) {
                if (is_id_char(src[k])) {
                    id += src[k];
                } else {
                    break;
                }
            }
            i = k;
            auto tok = new Token { Token::Type::Identifier, Variant(id, Type::String) };
            std::cout << "→ Found token: " << tok->print() << std::endl;
            m_tokens.push_back(tok);
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
