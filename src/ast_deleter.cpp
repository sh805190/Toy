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

void ASTDeleter::Visit(Var* stmt) {
  if (stmt->initializer != nullptr) {
    DeleteAST(stmt->initializer);
  }
  delete stmt;
}

void ASTDeleter::Visit(Expr* expr) {
  //NO OP
  delete expr;
}

void ASTDeleter::Visit(Assign* expr) {
  DeleteAST(expr->value);
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

void ASTDeleter::Visit(Logical* expr) {
  DeleteAST(expr->lhs);
  DeleteAST(expr->rhs);
  delete expr;
}

void ASTDeleter::Visit(Unary* expr) {
  DeleteAST(expr->rhs);
  delete expr;
}

void ASTDeleter::Visit(Value* expr) {
  delete expr;
}

void ASTDeleter::Visit(Variable* expr) {
  delete expr;
}
