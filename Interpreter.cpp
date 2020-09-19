#include "Interpreter.h"

#include <iomanip>
#include <charconv>
#include <algorithm>

void Interpreter::handle_condition() {
    // pre stack: ?
    // post stack: bool containing the resulting value

    if (peek() == Token::OpeningParens) {
        consume(Token::OpeningParens);
        handle_condition();
        consume(Token::ClosingParens);
    } else {
        if (peek() == Token::True) {
            consume(Token::True);
            stack_push(true, Type::Bool);
        } else if (peek() == Token::False) {
            consume(Token::False);
            stack_push(false, Type::Bool);
        } else if (peek() == Token::Identifier) {
            handle();
        } else {
            error("invalid condition, unexpected token: " << nameof(peek()) << " at index " << m_index);
            m_ok = false;
            return;
        }
    }

    if (peek() == Token::And || peek() == Token::Or) {
        auto type = peek();
        consume_blindly();
        handle_condition();
        bool second = stack_top().as<bool>();
        stack_pop();
        bool first = stack_top().as<bool>();
        stack_pop();
        if (type == Token::And) {
            stack_push(first && second, Type::Bool);
        } else {
            stack_push(first || second, Type::Bool);
        }
    }
}

void Interpreter::handle_block() {
    m_scope_depth += 1;
    dbg("scope depth: " << m_scope_depth);
    consume(Token::OpeningCurly);
    size_t curlys = 1;
    while (m_index < m_tokens.size()) {
        if (peek() == Token::OpeningCurly) {
            curlys += 1;
        } else if (peek() == Token::ClosingCurly) {
            curlys -= 1;
        }
        if (curlys == 0) {
            break;
        } else {
            handle();
            if (m_tokens.at(m_index - 1)->type != Token::ClosingCurly) {
                consume(Token::Semicolon);
            }
        }
    }
    consume(Token::ClosingCurly);
    m_scope_depth -= 1;
    dbg("scope depth: " << m_scope_depth);
    clean_scope();
}

void Interpreter::handle_main() {
    m_scope_depth += 1;
    dbg("scope depth: " << m_scope_depth);
    while (m_index < m_tokens.size()) {
        handle();
        if (m_tokens.at(m_index - 1)->type != Token::ClosingCurly) {
            consume(Token::Semicolon);
        }
    }
    m_scope_depth -= 1;
    dbg("scope depth: " << m_scope_depth);
    clean_scope();
}

void Interpreter::handle_expression() {
    // pre stack: ?
    // post stack: the value(s) of the expression
    dbg(__FUNCTION__ << ": " << nameof(peek()));
    if (peek() == Token::StringLiteral) {
        dbg("string expression");
        handle_string_expression();
    } else if (peek() == Token::True
               || peek() == Token::False) {
        dbg("condition");
        handle_condition();
    } else if (peek() == Token::NumericLiteral
               || peek() == Token::Minus) {
        dbg("numeric expression");
        handle_numeric_expression();
    } else if (peek() == Token::Identifier) {
        if (m_index + 1 < m_tokens.size()
            && is_numeric_expression_part(m_tokens.at(m_index + 1)->type)) {
            dbg("identifier -> numeric expression");
            handle_numeric_expression();
        } else {
            dbg("identifier -> function");
            handle();
        }
    } else {
        error("Unknown expression kind: " << nameof(peek()));
    }
}

void Interpreter::handle_string_expression() {
    auto tok = m_tokens.at(m_index);
    consume(Token::Type::StringLiteral);
    stack_push(tok->value);
}

void Interpreter::handle_numeric_expression() {
    // find end of expression
    size_t parens = 0;
    bool end_found = false;
    std::vector<Variant> expr;
    for (; !end_found && m_index < m_tokens.size(); ++m_index) {
        dbg("current type at " << m_index << ": " << m_tokens.at(m_index)->print());
        switch (m_tokens.at(m_index)->type) {
        case Token::OpeningParens: {
            parens += 1;
            m_index += 1;
            handle_numeric_expression();
            m_index -= 1;
            expr.push_back(stack_top());
            stack_pop();
            break;
        }
        case Token::ClosingParens: {
            // parens only means end if they are closing and aren't part of the expression
            if (parens == 0) {
                //consume(Token::ClosingParens);
                end_found = true;
            } else {
                parens -= 1;
            }
            break;
        }
        case Token::Plus:
            expr.push_back(Variant(Op::Add, Type::Operator));
            break;
        case Token::Minus:
            expr.push_back(Variant(Op::Sub, Type::Operator));
            break;
        case Token::Star:
            expr.push_back(Variant(Op::Mult, Type::Operator));
            break;
        case Token::Slash:
            expr.push_back(Variant(Op::Div, Type::Operator));
            break;
        case Token::NumericLiteral:
            expr.push_back(m_tokens.at(m_index)->value);
            break;
        case Token::Identifier: {
            m_index--;
            handle();
            expr.push_back(stack_top());
            stack_pop();
            break;
        }
        default:
            end_found = true;
            break;
        }
    }
    --m_index;

    auto find_next_highest_precedence_operator = [&] {
        for (auto op : { Op::Mult, Op::Div, Op::Add, Op::Sub }) {
            auto iter = std::find_if(expr.begin(), expr.end(), [&](Variant& var) {
                return var.type() == Type::Operator && var.as<Op>() == op;
            });
            if (iter != expr.end()) {
                return iter;
            }
        }
        return expr.end();
    };

    for (auto var : expr) {
        dbg("\t" << var.print());
    }

    auto iter = find_next_highest_precedence_operator();
    while (iter != expr.end()) {
        if ((iter == expr.begin() || (iter - 1)->type() != Type::Number)
            && iter->as<Op>() == Op::Sub) {
            // sign -, not operator -, imitate `0-x`
            stack_push(Variant(0, Type::Number));
            stack_push(*iter);
            stack_push(*(iter + 1));
        } else {
            stack_push(*(iter - 1));
            stack_push(*iter);
            stack_push(*(iter + 1));
        }
        calculate();
        expr.insert(iter + 2, stack_top());
        expr.erase(iter - 1, iter + 2);
        stack_pop();
        iter = find_next_highest_precedence_operator();
    }

    stack_push(expr.front());
    expr.erase(expr.begin());

    if (!expr.empty()) {
        error("too many non-operators in numeric expression");
        for (auto var : expr) {
            dbg("\t" << var.print());
        }
        m_ok = false;
        return;
    }
}

void Interpreter::handle_variable_declaration(Type) {
    // get variable ID
    auto name = get_token();
    if (name.type != Token::Identifier) {
        error("expected identifier after typename");
        m_ok = false;
        return;
    }
    consume(Token::Identifier);
    auto name_literal = name.value.as<std::string>();
    consume(Token::Assign);
    handle_expression();
    if (!m_ok) {
        return;
    }
    // now we should have a value on the stack!
    if (stack_empty()) {
        error("cannot assign void");
        m_ok = false;
        return;
    }
    // and there should be no variable with the same name
    if (m_variables.find(name_literal) != m_variables.end()) {
        error("cannot declare variable - variable with same name already exists!");
        m_ok = false;
        return;
    }
    // all ok, now store
    m_variables[name_literal] = Variable { stack_top(), m_scope_depth };
    stack_pop();
}

void Interpreter::skip_block() {
    consume(Token::OpeningCurly);
    size_t curlys = 1;
    for (;;) {
        if (peek() == Token::OpeningCurly) {
            curlys += 1;
        } else if (peek() == Token::ClosingCurly) {
            curlys -= 1;
        }
        if (curlys == 0) {
            break;
        } else {
            // do nothing, as we just skip!
            consume_blindly();
        }
    }
    consume(Token::ClosingCurly);
}

void Interpreter::handle() {
    // pre stack: ?
    // post stack: ?

    if (!m_ok) {
        return;
    }
    const Token& tok = *m_tokens.at(m_index);
    ++m_index;
    switch (tok.type) {
    case Token::If: {
        consume(Token::OpeningParens);
        handle_condition();
        consume(Token::ClosingParens);
        bool condition = stack_top().as<bool>();
        stack_pop();
        if (condition == true) {
            handle_block();
        } else {
            skip_block();
        }
        break;
    }
    case Token::While: {
        consume(Token::OpeningParens);
        handle_condition();
        consume(Token::ClosingParens);
        bool condition = stack_top().as<bool>();
        stack_pop();
        size_t start_of_block = m_index;
        std::optional<size_t> end_of_block = std::nullopt;
        while (condition == true) {
            handle_block();
            end_of_block = m_index;
            m_index = start_of_block;
        }
        if (end_of_block.has_value()) {
            m_index = end_of_block.value();
        } else {
            skip_block();
        }
        //skip_block();
        break;
    }
    case Token::Identifier: {
        auto val = tok.value.as<std::string>();
        if (m_function_map.find(val) != m_function_map.end()) {
            // function call!
            // get args
            auto after = call_function(val, m_index);
            m_index = after;
        } else if (m_type_map.find(val) != m_type_map.end()) {
            // variable declaration!
            handle_variable_declaration(m_type_map[val]);
        } else if (m_variables.find(val) != m_variables.end()) {
            stack_push(m_variables[val].value);
        } else {
            error("identifier \"" << val << "\" doesn't map to any function or value");
            m_ok = false;
            return;
        }
        break;
    }
    default:
        error("invalid token detected: " << tok.print());
        m_ok = false;
        return;
    }
}

void Interpreter::expect_token(Token::Type type) {
    if (m_index >= m_tokens.size()) {
        error("Expected " + nameof(type) + ", got end of code instead, at index " + std::to_string(m_index));
        m_ok = false;
    } else if (m_tokens.at(m_index)->type != type) {
        error("Expected " + nameof(type) + ", got " + nameof(m_tokens[m_index]->type) + " (\"" + m_tokens[m_index]->print() + "\") instead, at index " + std::to_string(m_index));
        m_ok = false;
    }
}

void Interpreter::consume(Token::Type type) {
    if (m_index >= m_tokens.size() || m_tokens.at(m_index)->type != type) {
        expect_token(type);
    }
    ++m_index;
}

void Interpreter::consume_blindly() {
    ++m_index;
}

void Interpreter::clean_scope() {
    auto iter = m_variables.begin();
    while ((iter = std::find_if(m_variables.begin(), m_variables.end(), [&](auto& pair) {
        auto var = pair.second;
        return var.scope > m_scope_depth;
    })) != m_variables.end()) {
        m_variables.erase(iter);
    }
}

size_t Interpreter::call_function(const std::string& val, size_t index) {
    const size_t old_index = m_index;
    m_index = index;
    size_t argc = gather_args();
    if (argc != m_function_map[val].argc) {
        error("\"" << val << "\" expects " << m_function_map[val].argc << " arguments, got " << argc << " instead.");
        m_ok = false;
        m_index = old_index;
        return m_index;
    } else {
        m_function_map[val].fn();
    }
    auto ret = m_index;
    m_index = old_index;
    return ret;
}


const Token& Interpreter::get_token() {
    return *m_tokens[m_index];
}

[[nodiscard]] size_t Interpreter::gather_args() {
    consume(Token::OpeningParens);
    size_t parens = 1;
    size_t args = 0;
    for (;;) {
        if (peek() == Token::OpeningParens) {
            parens += 1;
        } else if (peek() == Token::ClosingParens) {
            parens -= 1;
        }
        if (parens == 0) {
            break;
        } else {
            dbg("=> before handle expr");
            handle_expression();
            dbg("=> after handle expr");
            ++args;
            if (peek() != Token::Comma) {
                break;
            } else {
                consume(Token::Comma);
            }
        }
    }
    consume(Token::ClosingParens);
    return args;
}

bool Interpreter::is_numeric_expression_part(Token::Type t) {
    return t == Token::Plus
           || t == Token::Minus
           || t == Token::Star
           || t == Token::Slash;
}

void Interpreter::calculate() {
    // pre: 3 elements on the stack
    // post: stack shrunk by 3 elements
    // executes a calculation that is layed out on the stack
    auto size_before = stack_size();
    auto res = stack_expect(Type::Number, Type::Operator, Type::Number);
    if (res.has_value()) {
        auto [left_var, op_var, right_var] = res.value();
        double left = left_var.as<double>();
        Op op = op_var.as<Op>();
        double right = right_var.as<double>();
        switch (op) {
        case Op::Add:
            dbg(left << " + " << right);
            stack_push(left + right, Type::Number);
            break;
        case Op::Sub:
            dbg(left << " - " << right);
            stack_push(left - right, Type::Number);
            break;
        case Op::Mult:
            dbg(left << " * " << right);
            stack_push(left * right, Type::Number);
            break;
        case Op::Div:
            dbg(left << " / " << right);
            stack_push(left / right, Type::Number);
            break;
        default:
            error("invalid op: " << op_to_string(op));
            m_ok = false;
            return;
        }
    } else {
        error("stack does not contain elements expected for calculation");
        m_ok = false;
        return;
    }

    if (size_before - stack_size() != 2) {
        error("stack was not cleared, post condition failed in calculate");
        m_ok = false;
        return;
    }
}


Token::Type Interpreter::peek() const {
    if (m_index >= m_tokens.size()) {
        return Token::Type::Invalid;
    }
    return m_tokens.at(m_index)->type;
}

void Interpreter::print_stack(const std::string& where) const {
#if DBG
    std::cout << " > stack " << where << "\n";
    std::stack stack_copy = m_stack;
    size_t i = 0;
    while (!stack_copy.empty()) {
        std::cout << "    ";
        if (i == 0) {
            std::cout << "top: ";
        } else {
            std::cout << std::setw(3) << i << ": ";
        }
        std::cout << stack_copy.top().print() << "\n";
        stack_copy.pop();
        ++i;
    }
    std::cout << std::endl;
#endif
}

Interpreter::Interpreter(std::vector<Token*>& tokens)
    : m_tokens(tokens) {
    auto fn_print = [&] {
        auto res = stack_expect(Type::String);
        if (res.has_value()) {
            auto [str] = res.value();
            std::cout << str.as<std::string>() << std::endl;
        } else {
            error("call to print failed, invalid args");
            m_ok = false;
            return;
        }
    };

    auto fn_string_concat = [&] {
        auto res = stack_expect(Type::String, Type::String);
        if (res.has_value()) {
            auto [str1, str2] = res.value();
            stack_push(str1.as<std::string>() + str2.as<std::string>(), Type::String);
        } else {
            error("call to string_concat failed, invalid args");
            m_ok = false;
            return;
        }
    };

    auto fn_number_to_string = [&] {
        auto res = stack_expect(Type::Number);
        if (res.has_value()) {
            auto [num] = res.value();
            std::string str;
            str.resize(30);
            auto n = std::sprintf(str.data(), "%f", num.as<double>());
            stack_push(str.substr(0, n), Type::String);
        } else {
            error("call to number_to_string failed, invalid args");
            m_ok = false;
            return;
        }
    };

    // map of name to (function, arg_count)
    m_function_map = {
        { "print", { fn_print, 1 } },
        { "string_concat", { fn_string_concat, 2 } },
        { "number_to_string", { fn_number_to_string, 1 } },
    };

    m_type_map = {
        { "Number", Type::Number },
        { "String", Type::String },
        { "Array", Type::Array },
        { "Bool", Type::Bool },
    };

    m_variables = {
        { "PI", { Variant(double(3.1415926535897932384626433832795028841), Type::Number), GLOBAL_SCOPE } },
    };
}


Result Interpreter::run() {
    m_index = 0;
    while (m_ok && m_index < m_tokens.size()) {
        handle_main();
    }

    print_stack("at end of program");
    return { m_ok };
}
