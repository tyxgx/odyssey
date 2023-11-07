#include "ast.h"

llvm::Value *IntLiteralExpr::codegen(CodeVisitor &visitor) {
    return visitor.VisitIntLiteral(const_cast<IntLiteralExpr &>(*this));
}

llvm::Value *DoubleLiteralExpr::codegen(CodeVisitor &visitor) {
    return visitor.VisitDoubleLiteral(const_cast<DoubleLiteralExpr &>(*this));
}

llvm::Value *BoolLiteralExpr::codegen(CodeVisitor &visitor) {
    return visitor.VisitBoolLiteral(const_cast<BoolLiteralExpr &>(*this));
}

llvm::Value *BinaryExpr::codegen(CodeVisitor &visitor) {
    return visitor.VisitBinaryExpr(const_cast<BinaryExpr &>(*this));
}

llvm::Value *StringLiteralExpr::codegen(CodeVisitor &visitor) {
    return visitor.VisitStringLiteral(const_cast<StringLiteralExpr &>(*this));
}

llvm::Value *UnaryExpr::codegen(CodeVisitor &visitor) {
    return visitor.VisitUnaryExpr(const_cast<UnaryExpr &>(*this));
}
