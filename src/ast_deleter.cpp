#include "ast_deleter.hpp"

void ASTDeleter::DeleteAST(Expr* expr) {
  expr->Accept(this);
}

void ASTDeleter::DeleteAST(Stmt* stmt) {
  stmt->Accept(this);
}

void ASTDeleter::Visit(Stmt* stmt) {
  //NO OP
  delete stmt;
}

void ASTDeleter::Visit(Expression* stmt) {
  DeleteAST(stmt->expr);
  delete stmt;
}

void ASTDeleter::Visit(Expr* expr) {
  //NO OP
  delete expr;
}

void ASTDeleter::Visit(Binary* expr) {
  DeleteAST(expr->lhs);
  DeleteAST(expr->rhs);
  delete expr;
}

void ASTDeleter::Visit(Grouping* expr) {
  DeleteAST(expr->inner);
  delete expr;
}

void ASTDeleter::Visit(Unary* expr) {
  DeleteAST(expr->rhs);
  delete expr;
}

void ASTDeleter::Visit(Value* expr) {
  delete expr;
}

