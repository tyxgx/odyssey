#ifndef OD_AST_H
#define OD_AST_H

#include <iostream>
#include <string>

#include "code_visitor.h"
#include "errors.h"
#include "lexer.h"

class IntLiteralExpr;
class DoubleLiteralExpr;
class BoolLiteralExpr;
class StringLiteralExpr;
/* class NilLiteralExpr; */
class BinaryExpr;
class UnaryExpr;
class GroupingExpr;
class VariableExpr;
class VariableStmt;
class Expression;

class Expr {
   public:
    virtual ~Expr() = default;
    virtual llvm::Value *codegen(CodeVisitor &) = 0;
};

class IntLiteralExpr : public Expr {
   public:
    int Content;
    int Line;
    size_t StartsAt, EndsAt;
    IntLiteralExpr(int content, int line, size_t starts_at, size_t ends_at)
        : Content(content), Line(line), StartsAt(starts_at), EndsAt(ends_at){};
    llvm::Value *codegen(CodeVisitor &) override;
};

class VariableExpr : public Expr {
   public:
    std::string Name;
    int Line;
    size_t StartsAt, EndsAt;
    VariableExpr(std::string name, int line, size_t starts_at, size_t ends_at)
        : Name(name), Line(line), StartsAt(starts_at), EndsAt(ends_at){};
    llvm::Value *codegen(CodeVisitor &) override;
};

class DoubleLiteralExpr : public Expr {
   public:
    double Content;
    int Line;
    size_t StartsAt, EndsAt;
    DoubleLiteralExpr(double content, int line, size_t starts_at,
                      size_t ends_at)
        : Content(content), Line(line), StartsAt(starts_at), EndsAt(ends_at){};
    llvm::Value *codegen(CodeVisitor &) override;
};

class StringLiteralExpr : public Expr {
   public:
    std::string Content;
    int Line;
    size_t StartsAt, EndsAt;
    StringLiteralExpr(std::string content, int line, size_t starts_at,
                      size_t ends_at)
        : Content(content), Line(line), StartsAt(starts_at), EndsAt(ends_at){};
    llvm::Value *codegen(CodeVisitor &) override;
};

class BoolLiteralExpr : public Expr {
   public:
    bool Content;
    int Line;
    size_t StartsAt, EndsAt;
    BoolLiteralExpr(bool content, int line, size_t starts_at, size_t ends_at)
        : Content(content), Line(line), StartsAt(starts_at), EndsAt(ends_at){};
    llvm::Value *codegen(CodeVisitor &) override;
};

class UnaryExpr : public Expr {
   public:
    enum TokenKind Operator;
    std::unique_ptr<Expr> RHS;
    int Line;
    size_t StartsAt, EndsAt;
    UnaryExpr(enum TokenKind op, std::unique_ptr<Expr> rhs, int line,
              size_t starts_at, size_t ends_at)
        : Operator(op),
          RHS(std::move(rhs)),
          Line(line),
          StartsAt(starts_at),
          EndsAt(ends_at){};
    llvm::Value *codegen(CodeVisitor &) override;
};

class BinaryExpr : public Expr {
   public:
    enum TokenKind Operator;
    std::unique_ptr<Expr> LHS, RHS;
    int Line;
    size_t StartsAt, EndsAt;
    BinaryExpr(enum TokenKind op, std::unique_ptr<Expr> lhs,
               std::unique_ptr<Expr> rhs, int line, size_t starts_at,
               size_t ends_at)
        : Operator(op),
          LHS(std::move(lhs)),
          RHS(std::move(rhs)),
          Line(line),
          StartsAt(starts_at),
          EndsAt(ends_at){};
    llvm::Value *codegen(CodeVisitor &) override;
};

class GroupingExpr : public Expr {
    std::unique_ptr<Expr> Exp;

   public:
    GroupingExpr(std::unique_ptr<Expr> exp) : Exp(std::move(exp)){};
};

/* class NilLiteralExpr : public Expr { */
/*    public: */
/*       NilLiteralExpr() : NilValue(TOKEN_NIL){}; */
/*       llvm::Value *codegen(CodeVisitor &) override; */
/* }; */

class Stmt {
   public:
    virtual ~Stmt() = default;
    virtual llvm::Value *codegen(CodeVisitor &) = 0;
};

// inheritance has to be public, not private!!
class Expression : public Stmt {
   public:
    std::unique_ptr<Expr> E;
    int Line;
    size_t StartsAt, EndsAt;
    Expression(std::unique_ptr<Expr> expression, int line, size_t starts_at,
                   size_t ends_at)
        : E(std::move(expression)),
          Line(line),
          StartsAt(starts_at),
          EndsAt(starts_at){};
    llvm::Value *codegen(CodeVisitor &) override;
};

class VariableStmt : public Stmt {
   public:
    std::string Name;
    std::unique_ptr<Stmt> Decl;
    int Line;
    size_t StartsAt, EndsAt;
    VariableStmt(std::string name, std::unique_ptr<Stmt> decl, int line,
                 size_t starts_at, size_t ends_at)
        : Name(name),
          Decl(std::move(decl)),
          Line(line),
          StartsAt(starts_at),
          EndsAt(ends_at){};
    llvm::Value *codegen(CodeVisitor &) override;
};

#endif
