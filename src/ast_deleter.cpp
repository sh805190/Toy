#include "ast_deleter.hpp"

void ASTDeleter::DeleteAST(Expr* expr) {
  if (expr) {
    expr->Accept(this);
  }
}

void ASTDeleter::DeleteAST(Stmt* stmt) {
  if (stmt) {
    stmt->Accept(this);
  }
}

void ASTDeleter::Visit(Stmt* stmt) {
  //NO OP
  delete stmt;
}

void ASTDeleter::Visit(Block* stmt) {
  for(Stmt* ptr : stmt->stmtList) {
    DeleteAST(ptr);
  }
  delete stmt;
}

void ASTDeleter::Visit(Expression* stmt) {
  DeleteAST(stmt->expr);
  delete stmt;
}

void ASTDeleter::Visit(If* stmt) {
  DeleteAST(stmt->condition);
  DeleteAST(stmt->thenBranch);
  DeleteAST(stmt->elseBranch);
  delete stmt;
}

void ASTDeleter::Visit(Var* stmt) {
  DeleteAST(stmt->initializer);
  delete stmt;
}

void ASTDeleter::Visit(While* stmt) {
  DeleteAST(stmt->condition);
  DeleteAST(stmt->branch);
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
