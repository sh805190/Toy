#include "ast_reader.hpp"

#include <iostream>

//prefix methods
void ASTReaderPrefix::Print(Stmt* stmt) {
  std::cout << "(";
  stmt->Accept(this);
  std::cout << ")";
}

void ASTReaderPrefix::Print(Expr* expr) {
  expr->Accept(this);
}

void ASTReaderPrefix::Visit(Stmt* stmt) {
  std::cout << "STMT";
}

void ASTReaderPrefix::Visit(Expression* stmt) {
  Print(stmt->expr);
}

void ASTReaderPrefix::Visit(Expr* expr) {
  std::cout << "EXPR";
}

void ASTReaderPrefix::Visit(Binary* expr) {
  std::cout << expr->op.GetLexeme() << " ";
  Print(expr->lhs);
  std::cout << " ";
  Print(expr->rhs);
}

void ASTReaderPrefix::Visit(Grouping* expr) {
  std::cout << "(";
  Print(expr->inner);
  std::cout << ")";
}

void ASTReaderPrefix::Visit(Logical* expr) {
  std::cout << expr->op.GetLexeme() << " ";
  Print(expr->lhs);
  std::cout << " ";
  Print(expr->rhs);
}

void ASTReaderPrefix::Visit(Unary* expr) {
  std::cout << expr->op.GetLexeme();
  Print(expr->rhs);
}

void ASTReaderPrefix::Visit(Value* expr) {
  std::cout << expr->value.ToString();
}

//postfix methods
void ASTReaderPostfix::Print(Stmt* stmt) {
  stmt->Accept(this);
}

void ASTReaderPostfix::Print(Expr* expr) {
  expr->Accept(this);
}

void ASTReaderPostfix::Visit(Stmt* stmt) {
  std::cout << "STMT";
}

void ASTReaderPostfix::Visit(Expression* stmt) {
  Print(stmt->expr);
}

void ASTReaderPostfix::Visit(Expr* expr) {
  std::cout << "EXPR";
}

void ASTReaderPostfix::Visit(Binary* expr) {
  Print(expr->lhs);
  std::cout << " ";
  Print(expr->rhs);
  std::cout << " " << expr->op.GetLexeme();
}

void ASTReaderPostfix::Visit(Grouping* expr) {
  std::cout << "(";
  Print(expr->inner);
  std::cout << ")";
}

void ASTReaderPostfix::Visit(Logical* expr) {
  Print(expr->lhs);
  std::cout << " ";
  Print(expr->rhs);
  std::cout << " " << expr->op.GetLexeme();
}

void ASTReaderPostfix::Visit(Unary* expr) {
  if (expr->op.GetType() == BANG || expr->op.GetType() == MINUS) {
    Print(expr->rhs);
    std::cout << " negate";
  }
  else {
    Print(expr->rhs);
    std::cout << expr->op.GetLexeme();
  }
}

void ASTReaderPostfix::Visit(Value* expr) {
  std::cout << expr->value.ToString();
}

