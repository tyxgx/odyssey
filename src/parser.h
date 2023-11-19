#ifndef OD_PARSER_H
#define OD_PARSER_H

// Parser includes
#include <unordered_map>
#include <vector>

#include "ast.h"
#include "errors.h"
#include "lexer.h"

class Parser {
    struct Token _advance();
    struct Token _peek();
    struct Token _previous();
    struct Token _consume(enum TokenKind expected, std::string msg);

    template <enum TokenKind curr, enum TokenKind... rest>
    bool _match();

    bool _check(enum TokenKind expected);
    bool _at_eof();
    int _get_precedence();
    void _report_error(std::string message);

    // variables
    std::vector<struct Token> _token_list;  // passed in from lexer
    size_t _current_ptr;  // index points to the current position in the list
    bool _error_occurred;
    std::unordered_map<enum TokenKind, std::pair<std::string, int>>
        _binop_precedence;

    // Ast builders
    std::unique_ptr<Expr> _expression();
    std::unique_ptr<Expr> _parse_primary_expr(bool from_binary_expression);
    std::unique_ptr<Expr> _parse_binary_expr(int min_prec,
                                             std::unique_ptr<Expr> LHS);
    std::unique_ptr<Expr> _parse_paren_expr();
    std::unique_ptr<Expr> _parse_unary_expr();
    std::unique_ptr<Stmt> _parse_statement();
    std::unique_ptr<Stmt> _parse_var_decl();
    std::unique_ptr<Stmt> _parse_declaration();

   public:
    std::vector<struct Diagnostic> _diagnostics;
    std::vector<std::unique_ptr<Stmt>> parse();
    Parser(std::vector<struct Token> token_list);
};

#endif
