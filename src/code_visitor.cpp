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
}

llvm::Value *CodeVisitor::VisitIntLiteral(IntLiteralExpr &ast_node) {
    return llvm::ConstantInt::get(*TheContext,
                                  llvm::APInt(32U, (uint64_t)ast_node.Content));
}

llvm::Value *CodeVisitor::VisitStringLiteral(StringLiteralExpr &ast_node) {
    return Builder->CreateGlobalStringPtr(ast_node.Content);
}

llvm::Value *CodeVisitor::VisitDoubleLiteral(DoubleLiteralExpr &ast_node) {
    return llvm::ConstantFP::get(*TheContext, llvm::APFloat(ast_node.Content));
}

llvm::Value *CodeVisitor::VisitBoolLiteral(BoolLiteralExpr &ast_node) {
    return llvm::ConstantInt::getBool(*TheContext, ast_node.Content);
}

llvm::Value *CodeVisitor::VisitUnaryExpr(UnaryExpr &ast_node) {
    llvm::Value *rhs = ast_node.RHS->codegen(*this);
    if (ast_node.Operator == TOKEN_MINUS) {
        return Builder->CreateNeg(rhs);
    }
    return rhs;
}

llvm::Value *CodeVisitor::VisitBinaryExpr(BinaryExpr &ast_node) {
    llvm::Value *lhs_val = ast_node.LHS->codegen(*this);
    llvm::Value *rhs_val = ast_node.RHS->codegen(*this);
    if (!lhs_val || !rhs_val)
        std::cerr << "could not resolve LHS and RHS in binary expression"
                  << std::endl;

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
}
