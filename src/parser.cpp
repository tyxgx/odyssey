// Parser mixes Recursive descent and precedence climbing
#include "parser.h"

#include <assert.h>
#include <ctype.h>

#include <memory>
#include <string>

// construct
Parser::Parser(std::vector<struct Token> token_list) {
    _binop_precedence[TOKEN_EQUAL_EQUAL] = std::make_pair("left", 10);
    _binop_precedence[TOKEN_BANG_EQUAL] = std::make_pair("left", 10);
    _binop_precedence[TOKEN_GREATER] = std::make_pair("left", 20);
    _binop_precedence[TOKEN_GREATER_EQUAL] = std::make_pair("left", 20);
    _binop_precedence[TOKEN_LESS] = std::make_pair("left", 20);
    _binop_precedence[TOKEN_LESS_EQUAL] = std::make_pair("left", 20);
    _binop_precedence[TOKEN_PLUS] = std::make_pair("left", 30);
    _binop_precedence[TOKEN_MINUS] = std::make_pair("left", 30);
    _binop_precedence[TOKEN_SLASH] = std::make_pair("left", 40);
    _binop_precedence[TOKEN_STAR] = std::make_pair("left", 40);
    _binop_precedence[TOKEN_BANG] = std::make_pair("left", 50);  // highest.
    /* _binop_precedence[TOKEN_RIGHT_PAREN] = std::make_pair("none", 1);  //
     * dummy */
    /* _binop_precedence[TOKEN_LEFT_PAREN] = std::make_pair("none", 1);  //
     * dummy */
    _token_list = token_list;
    _current_ptr = 0;
    _error_occurred = false;
}

// first define the helper functions
struct Token Parser::_advance() {
    if (!_at_eof()) _current_ptr++;
    return _previous();
}

struct Token Parser::_peek() { return _token_list[_current_ptr]; }

struct Token Parser::_previous() { return _token_list[_current_ptr - 1]; }

bool Parser::_at_eof() { return _peek().tt == TOKEN_EOF; }

template <enum TokenKind curr, enum TokenKind... rest>
bool Parser::_match() {
    if (!_check(curr)) {
        // current token is not present in the list
        // if constexpr... is evaluated at compile time..
        if constexpr (sizeof...(rest) > 0) {
            return _match<rest...>();
        }
        // the list is finished
        return false;
    }
    // had a match, consume this token
    _advance();
    return true;
}

int Parser::_get_precedence() {
    int prec = _binop_precedence[_peek().tt].second;
    if (prec <= 0) return -1;
    return prec;
}

bool Parser::_check(enum TokenKind expected) { return _peek().tt == expected; }

struct Token Parser::_consume(enum TokenKind expected, std::string error_msg) {
    if (_check(expected)) return _advance();

    // Not found the expected token
    _report_error(error_msg);
    struct Token ret_token = {.tt = TOKEN_ERROR,
                              .len = 0,
                              .starts_at = 0,
                              .ends_at = 0,
                              .line = 0,
                              .content = "ERROR"};
    return ret_token;
}

void Parser::_report_error(std::string msg) {
    std::string report;
    if (_at_eof())
        report = " at the end of source code: " + msg;
    else
        report = _peek().dump() + "\b: " + msg;
    _error_occurred = true;

    struct Diagnostic diagnostic = {
        .starts_at = _peek().starts_at,
        .ends_at = _peek().ends_at,
        .line = _peek().line,
        .message = report,
    };
    _diagnostics.push_back(diagnostic);
}

// Parser functions
std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> stmts;
    std::unique_ptr<Stmt> s;
    while ((s = _parse_declaration()) != nullptr && !_at_eof()) {
    }
    stmts.push_back(std::move(s));

    return stmts;
}

std::unique_ptr<Stmt> Parser::_parse_declaration() {
    if (_peek().tt == TOKEN_LET) return _parse_var_decl();

    _report_error("unbound expression");
    return nullptr;
    /* return _parse_statement(); */
}

// variable_decl = "let" ID ('=' expression)? ';';
std::unique_ptr<Stmt> Parser::_parse_var_decl() {
    size_t start = _peek().starts_at;
    // consume the let
    _advance();
    // consume the variable's name
    std::string identifier = _consume(TOKEN_ID,
             "expected an identifier representing the name of the variable").content;
    std::unique_ptr<Stmt> expr = nullptr;
    if (_match<TOKEN_EQUAL>()) {
        expr = _parse_statement();
        if (!expr) {
            _report_error("expected expression following '=' after variable name");
            return nullptr;
        }
    }
    // doesn't matter if you std::move a nullptr
    return std::make_unique<VariableStmt>(identifier, std::move(expr), _previous().line, start, _previous().ends_at);
}

std::unique_ptr<Stmt> Parser::_parse_statement() {
    auto expression = _expression();

    if (!expression) return nullptr;

    _consume(TOKEN_SEMICOLON, "expect ';' to terminate expression");
    return std::make_unique<Expression>(std::move(expression), _previous().line,
                                  _previous().starts_at, _previous().ends_at);
}

std::unique_ptr<Expr> Parser::_expression() {
    auto LHS = _parse_unary_expr();
    if (!LHS) return nullptr;  // assert?
    // now, parse binary expression
    return _parse_binary_expr(0 /* The current min precedence */,
                              std::move(LHS));
}

std::unique_ptr<Expr> Parser::_parse_unary_expr() {
    enum TokenKind tt = _peek().tt;
    if ((tt != TOKEN_PLUS && tt != TOKEN_MINUS && tt != TOKEN_STAR &&
         tt != TOKEN_SLASH && tt != TOKEN_LESS && tt != TOKEN_SEMICOLON &&
         tt != TOKEN_RIGHT_PAREN) ||
        tt == TOKEN_LEFT_PAREN || tt == TOKEN_COMMA)
        return _parse_primary_expr(false);

    if (tt == TOKEN_RIGHT_PAREN) {
        _report_error("stray ')' in source");
        return nullptr;
    }

    _advance();
    enum TokenKind op = _previous().tt;

    if (auto RHS = _parse_unary_expr()) {
        return std::make_unique<UnaryExpr>(op, std::move(RHS), _previous().line,
                                           _previous().starts_at,
                                           _previous().ends_at);
    }
    return nullptr;
    /* return _parse_primary_expr(false); */
}

std::unique_ptr<Expr> Parser::_parse_primary_expr(bool from_binary_expr) {
    switch (_peek().tt) {
        /* case TOKEN_NIL: { */
        /*     _advance(); */
        /*     return std::make_unique<NilLiteralExpr>(); */
        /* } */
        case TOKEN_TRUE: {
            _advance();
            return std::make_unique<BoolLiteralExpr>(true, _previous().line,
                                                     _previous().starts_at,
                                                     _previous().ends_at);
        }
        case TOKEN_FALSE: {
            _advance();
            return std::make_unique<BoolLiteralExpr>(false, _previous().line,
                                                     _previous().starts_at,
                                                     _previous().ends_at);
        }
        case TOKEN_INT: {
            _advance();
            return std::make_unique<IntLiteralExpr>(
                std::stoi(_previous().content), _previous().line,
                _previous().starts_at, _previous().ends_at);
        }
        case TOKEN_DOUBLE: {
            _advance();
            return std::make_unique<DoubleLiteralExpr>(
                std::stod(_previous().content), _previous().line,
                _previous().starts_at, _previous().ends_at);
        }
        case TOKEN_STRING: {
            _advance();
            size_t len = _previous().len;
            std::string value = _previous().content.substr(1, len - 2);
            return std::make_unique<StringLiteralExpr>(value, _previous().line,
                                                       _previous().starts_at,
                                                       _previous().ends_at);
        }
        case TOKEN_LEFT_PAREN: {
            auto expr = _parse_paren_expr();
            return expr;
        }
        case TOKEN_RIGHT_PAREN:
            return nullptr;
        case TOKEN_ID:
            return std::make_unique<VariableExpr>(_previous().content, _previous().line, _previous().starts_at, _previous().ends_at);
        default: {
            if (!_at_eof()) {
                std::string report = "unexpected symbol '" + _peek().content;
                report += '\'';
                report += ", expected expression";
                _report_error(report);
            } else {
                std::string report = "unexpected symbol \'" +
                                     _previous().content +
                                     "\', expected expression";
                _report_error(report);
            }
            return nullptr;
        }
    }
}

std::unique_ptr<Expr> Parser::_parse_paren_expr() {
    // _match() consumed the '('
    // consume the left paren
    _advance();
    std::cout << "PAREN\n";
    auto expr = _expression();

    _consume(TOKEN_RIGHT_PAREN, "expect ')' after expression.");

    if (!expr) {
        _report_error("expected expression inside parentheses");
        return nullptr;
    }

    return expr;
}

std::unique_ptr<Expr> Parser::_parse_binary_expr(int min_prec,
                                                 std::unique_ptr<Expr> LHS) {
    // Precedence climbing is easy. From the precedence table,
    // get the current operator's precedence. evaluate next
    // operator's precedence. If current token has less than min precedence
    // return the expression wrapped in a ptr. otherwise, if next operator
    // has same precedence, loop to it. If it has a greater precedence,
    // recurse and make the current RHS the LHS for that operator
    while (true) {
        int current_token_prec = _get_precedence();
        if (current_token_prec < min_prec) return LHS;

        enum TokenKind op = _peek().tt;

        if (op == TOKEN_BANG) {
            _report_error("illegal use of '!' in expression");
        }

        // consume this operator
        _advance();
        // evaluate the expression on the RHS of the operator
        auto RHS = _parse_unary_expr();
        if (!RHS) return nullptr;

        int next_token_prec = _get_precedence();
        if (current_token_prec < next_token_prec) {
            // recurse
            RHS = _parse_binary_expr(current_token_prec + 1, std::move(RHS));
            // return if fails
            if (!RHS) return nullptr;
        }

        // merge
        LHS = std::make_unique<BinaryExpr>(
            op, std::move(LHS), std::move(RHS), _previous().line,
            _previous().starts_at, _previous().ends_at);
    }
}
