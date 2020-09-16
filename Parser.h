#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <memory>
#include "Token.h"

class Parser
{
private:
    std::vector<Token*> m_tokens;

public:
    Parser(const std::string& src);

    const std::vector<Token*>& tokens() const { return m_tokens; }
};

#endif // PARSER_H
