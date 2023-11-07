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

		IntLiteralExpr(int content) : Content(content) {};
		llvm::Value *codegen(CodeVisitor &) override;
};

class DoubleLiteralExpr : public Expr {
	public:
		double Content;

		DoubleLiteralExpr(double content) : Content(content) {};
		llvm::Value *codegen(CodeVisitor &) override;
};

class StringLiteralExpr : public Expr {
	public:
		std::string Content;

		StringLiteralExpr(std::string content) : Content(content) {};
		llvm::Value *codegen(CodeVisitor &) override;
};

class BoolLiteralExpr : public Expr {
	public:
		bool Content;

		BoolLiteralExpr(bool content) : Content(content) {};
		llvm::Value *codegen(CodeVisitor &) override;
};

class UnaryExpr : public Expr {
	public:
		enum TokenKind Operator;
		std::unique_ptr<Expr> RHS;

		UnaryExpr(enum TokenKind op, std::unique_ptr<Expr> rhs) : Operator(op), RHS(std::move(rhs)) {};
		llvm::Value *codegen(CodeVisitor &) override;
};

class BinaryExpr : public Expr {
	public:
		enum TokenKind Operator;
		std::unique_ptr<Expr> LHS, RHS;

		BinaryExpr(enum TokenKind op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs) : Operator(op), LHS(std::move(lhs)), RHS(std::move(rhs)) {};
		llvm::Value *codegen(CodeVisitor &) override;
};

class GroupingExpr : public Expr {
	std::unique_ptr<Expr> Exp;

	public:
	GroupingExpr(std::unique_ptr<Expr> exp) : Exp(std::move(exp)) {};
};

#endif
