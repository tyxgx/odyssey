#ifndef __OD_AST_H__
#define __OD_AST_H__

#include <string>
#include <iostream>

#include "lexer.h"
#include "errors.h"

class IntLiteralExpr;
class DoubleLiteralExpr;
class BoolLiteralExpr;
class StringLiteralExpr;
class NilLiteralExpr;
class BinaryExpr;
class UnaryExpr;
class GroupingExpr;
class VariableExpr;


class Expr {
	public:
		virtual ~Expr() = default;
};

class IntLiteralExpr : public Expr {
	int Content;

	public:
	IntLiteralExpr(int content) : Content(content) {};
};

class DoubleLiteralExpr : public Expr {
	double Content;

	public:
	DoubleLiteralExpr(double content) : Content(content) {};
};

class StringLiteralExpr : public Expr {
	std::string Content;

	public:
	StringLiteralExpr(std::string content) : Content(content) {};
};

class BoolLiteralExpr : public Expr {
	bool Content;

	public:
	BoolLiteralExpr(bool content) : Content(content) {};
};

class NilLiteralExpr : public Expr {
	enum TokenKind NilToken;

	public:
	NilLiteralExpr(enum TokenKind nil_token) : NilToken(nil_token) {};
};

class UnaryExpr : public Expr {
	enum TokenKind Operator;
	std::unique_ptr<Expr> RHS;

	public:
	UnaryExpr(enum TokenKind op, std::unique_ptr<Expr> rhs) : Operator(op), RHS(std::move(rhs)) {};
};

class BinaryExpr : public Expr {
	enum TokenKind Operator;
	std::unique_ptr<Expr> LHS, RHS;

	public:
	BinaryExpr(enum TokenKind op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs) : Operator(op), LHS(std::move(lhs)), RHS(std::move(rhs)) {};
};

class GroupingExpr : public Expr {
	std::unique_ptr<Expr> Exp;

	public:
	GroupingExpr(std::unique_ptr<Expr> exp) : Exp(std::move(exp)) {};
};

#endif
