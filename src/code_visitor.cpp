#include "code_visitor.h"

#include <iostream>

#include "ast.h"

// The approach we use here is to implement a Visitor Pattern to
// visit each of the subclasses accordingly.
// Then, generate LLVM IR as the classes are identified.

// Initialize the LLVM Module in the constructor of the Visitor.

CodeVisitor::CodeVisitor() noexcept {
    // init TheContext
    TheContext = std::make_unique<llvm::LLVMContext>();
    // create a unique pointer to the Module
    TheModule = std::make_unique<llvm::Module>("odyssey's jit", *TheContext);
    // init the IRBuilder
    Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
    had_error_somewhere = false;
}

llvm::Value *CodeVisitor::VisitIntLiteral(IntLiteralExpr &ast_node) {
    return llvm::ConstantInt::get(*TheContext,
                                  llvm::APInt(32U, (uint64_t)ast_node.Content));
}

llvm::Value *CodeVisitor::VisitStringLiteral(StringLiteralExpr &ast_node) {
    return Builder->CreateGlobalStringPtr(ast_node.Content);
}

llvm::Value *CodeVisitor::VisitDoubleLiteral(DoubleLiteralExpr &ast_node) {
    return llvm::ConstantFP::get(*TheContext,
                                 llvm::APFloat((double)(ast_node.Content)));
}

llvm::Value *CodeVisitor::VisitBoolLiteral(BoolLiteralExpr &ast_node) {
    return llvm::ConstantInt::getBool(*TheContext, ast_node.Content);
}

/* llvm::Value *CodeVisitor::VisitNilLiteral(BoolLiteralExpr &ast_node) { */
/*     return llvm::ConstantInt::getBool(*TheContext, ast_node.Content); */
/* } */

llvm::Value *CodeVisitor::VisitUnaryExpr(UnaryExpr &ast_node) {
    llvm::Value *rhs = ast_node.RHS->codegen(*this);
    auto type_rhs = rhs->getType();
    if (ast_node.Operator == TOKEN_MINUS &&
        (type_rhs->isDoubleTy() || type_rhs->isIntegerTy())) {
        if (type_rhs->isDoubleTy())
            return Builder->CreateFSub(
                llvm::ConstantFP::get(*TheContext, llvm::APFloat((double)0.0)),
                rhs);
        else
            return Builder->CreateSub(
                llvm::ConstantInt::get(*TheContext, llvm::APInt(32U, 0U)), rhs);
    } else if (type_rhs->isDoubleTy() || type_rhs->isIntegerTy()) {
        return rhs;
    }
    report_error_("illegal operand for unary minus", ast_node.Line,
                  ast_node.StartsAt, ast_node.EndsAt);
    return nullptr;
}

llvm::Value* CodeVisitor::VisitVariableExpr(VariableExpr &ast_node) {
    auto value = NamedValues[ast_node.Name];
    if(!value) {
        report_error_("variable not given a value, but used in expression", ast_node.Line, ast_node.StartsAt, ast_node.EndsAt);
    }
    return value;
}

llvm::Value *CodeVisitor::VisitBinaryExpr(BinaryExpr &ast_node) {
    llvm::Value *lhs_val = ast_node.LHS->codegen(*this);
    llvm::Value *rhs_val = ast_node.RHS->codegen(*this);
    if (!lhs_val || !rhs_val)
        report_error_(
            "could not resolve left and right hand sides in binary expression",
            ast_node.Line, ast_node.StartsAt, ast_node.EndsAt);

    auto type_lhs = lhs_val->getType();
    auto type_rhs = rhs_val->getType();

    if (type_lhs->getTypeID() != type_rhs->getTypeID()) {
        report_error_(
            "arguments of different types provided to binary expression; "
            "implicit casts not allowed",
            ast_node.Line, ast_node.StartsAt, ast_node.EndsAt);
    }

    if (type_lhs->isDoubleTy()) {
        switch (ast_node.Operator) {
            case TOKEN_PLUS:
                return Builder->CreateFAdd(lhs_val, rhs_val, "addtmp");
            case TOKEN_MINUS:
                return Builder->CreateFSub(lhs_val, rhs_val, "subtmp");
            case TOKEN_STAR:
                return Builder->CreateFMul(lhs_val, rhs_val, "multmp");
            case TOKEN_SLASH:
                return Builder->CreateFDiv(lhs_val, rhs_val, "divtmp");
            case TOKEN_LESS:
                lhs_val = Builder->CreateFCmpULT(lhs_val, rhs_val, "cmptmp");
                // Convert bool 0/1 to double 0.0 or 1.0
                return Builder->CreateUIToFP(
                    lhs_val, llvm::Type::getDoubleTy(*TheContext), "booltmp");
            default:
                std::cerr << "TODO" << std::endl;
                return nullptr;
        }
    } else if (type_lhs->isIntegerTy()) {
        switch (ast_node.Operator) {
            case TOKEN_PLUS:
                return Builder->CreateAdd(lhs_val, rhs_val, "addtmp");
            case TOKEN_MINUS:
                return Builder->CreateSub(lhs_val, rhs_val, "subtmp");
            case TOKEN_STAR:
                return Builder->CreateMul(lhs_val, rhs_val, "multmp");
            case TOKEN_SLASH:
                return Builder->CreateSDiv(lhs_val, rhs_val, "divtmp");
            case TOKEN_LESS:
                lhs_val = Builder->CreateFCmpULT(lhs_val, rhs_val, "cmptmp");
                // Convert bool 0/1 to double 0.0 or 1.0
                return Builder->CreateUIToFP(
                    lhs_val, llvm::Type::getDoubleTy(*TheContext), "booltmp");
            default:
                std::cerr << "TODO" << std::endl;
                return nullptr;
        }
    } else {
        report_error_(
            "non-numerical operands not allowed in binary expressions",
            ast_node.Line, ast_node.StartsAt, ast_node.EndsAt);
    }
    return nullptr;
}

llvm::Value *CodeVisitor::VisitExpression(Expression &ast_node) {
    auto expr = ast_node.E->codegen(*this);
    if (!expr) {
        report_error_("illegal expression", ast_node.Line, ast_node.StartsAt,
                      ast_node.EndsAt);
        return nullptr;
    }
    return expr;
}

llvm::Value *CodeVisitor::VisitVariableStmt(VariableStmt &ast_node) {
    llvm::Value *expr = nullptr;
    if(ast_node.Decl)
        expr = ast_node.Decl->codegen(*this);

    NamedValues[ast_node.Name] = expr;
    return expr;
}

void CodeVisitor::report_error_(std::string message, int line, size_t starts_at,
                                size_t ends_at) {
    std::string error = "[line " + std::to_string(line) + "] from col " +
                        std::to_string(starts_at) + " to col " +
                        std::to_string(ends_at) + ": " + message;
    struct Diagnostic diagnostic = {
        .starts_at = 24,
        .ends_at = 24,
        .line = line,
        .message = error,
    };
    diagnostics_.push_back(diagnostic);
    had_error_somewhere = true;
}
