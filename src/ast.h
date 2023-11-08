#ifndef OD_AST_H
#define OD_AST_H

#include <string>
#include <iostream>

#include "lexer.h"
#include "code_visitor.h"
#include "errors.h"

class IntLiteralExpr;
class DoubleLiteralExpr;
class BoolLiteralExpr;
class StringLiteralExpr;
/* class NilLiteralExpr; */
class BinaryExpr;
class UnaryExpr;
class GroupingExpr;
class VariableExpr;


class Expr {
	public:
		virtual ~Expr() = default;
		virtual llvm::Value *codegen(CodeVisitor &) = 0;
};

class IntLiteralExpr : public Expr {
	public:
		int Content;
		int Line;
		IntLiteralExpr(int content, int line) : Content(content), Line(line) {};
		llvm::Value *codegen(CodeVisitor &) override;
};

class DoubleLiteralExpr : public Expr {
	public:
		double Content;
		int Line;
		DoubleLiteralExpr(double content, int line) : Content(content), Line(line) {};
		llvm::Value *codegen(CodeVisitor &) override;
};

class StringLiteralExpr : public Expr {
	public:
		std::string Content;
		int Line;
		StringLiteralExpr(std::string content, int line) : Content(content), Line(line) {};
		llvm::Value *codegen(CodeVisitor &) override;
};

class BoolLiteralExpr : public Expr {
	public:
		bool Content;
		int Line;
		BoolLiteralExpr(bool content, int line) : Content(content), Line(line) {};
		llvm::Value *codegen(CodeVisitor &) override;
};

class UnaryExpr : public Expr {
	public:
		enum TokenKind Operator;
		std::unique_ptr<Expr> RHS;
		int Line;
		UnaryExpr(enum TokenKind op, std::unique_ptr<Expr> rhs, int line) : Operator(op), RHS(std::move(rhs)), Line(line) {};
		llvm::Value *codegen(CodeVisitor &) override;
};

class BinaryExpr : public Expr {
	public:
		enum TokenKind Operator;
		std::unique_ptr<Expr> LHS, RHS;
		int Line;
		BinaryExpr(enum TokenKind op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs, int line) : Operator(op), LHS(std::move(lhs)), RHS(std::move(rhs)), Line(line) {};
		llvm::Value *codegen(CodeVisitor &) override;
};

class GroupingExpr : public Expr {
	std::unique_ptr<Expr> Exp;

	public:
	GroupingExpr(std::unique_ptr<Expr> exp) : Exp(std::move(exp)) {};
};

#endif
