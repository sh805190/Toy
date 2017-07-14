#include "ast_duplicator.hpp"

#include "runtime_error.hpp"

#include <iostream>

Expr* ASTDuplicator::DuplicateAST(Expr* expr) {
  if (expr == nullptr) {
    return nullptr;
  }

  expr->Accept(this);
  return resultExpr;
}

Stmt* ASTDuplicator::DuplicateAST(Stmt* stmt) {
  if (stmt == nullptr) {
    return nullptr;
  }

  stmt->Accept(this);
  return resultStmt;
}

void ASTDuplicator::Visit(Stmt* stmt) {
  //Nothing can be done
  throw RuntimeError(-1, "Can't duplicate empty statement");
}

void ASTDuplicator::Visit(Block* stmt) {
  std::list<Stmt*> stmtList;
  for (Stmt* it : stmt->stmtList) {
    DuplicateAST(it);
    stmtList.push_back(resultStmt);
  }
  resultStmt = new Block(stmtList);
std::cout << "\tBLOCK DUPLICATED" << std::endl;
}

void ASTDuplicator::Visit(Break* stmt) {
  resultStmt = new Break(stmt->line);
}

void ASTDuplicator::Visit(Continue* stmt) {
  resultStmt = new Continue(stmt->line);
}

void ASTDuplicator::Visit(Expression* stmt) {
  DuplicateAST(stmt->expr);
  resultStmt = new Expression(resultExpr);
}

void ASTDuplicator::Visit(If* stmt) {
  DuplicateAST(stmt->condition);
  Expr* condition = resultExpr;

  DuplicateAST(stmt->thenBranch);
  Stmt* thenBranch = resultStmt;

  DuplicateAST(stmt->elseBranch);
  Stmt* elseBranch = resultStmt;

  resultStmt = new If(condition, thenBranch, elseBranch);
}

void ASTDuplicator::Visit(Return* stmt) {
  DuplicateAST(stmt->result);
  resultStmt = new Return(stmt->line, resultExpr);
}

void ASTDuplicator::Visit(Var* stmt) {
  DuplicateAST(stmt->initializer);
  resultStmt = new Var(stmt->name, resultExpr);
}

void ASTDuplicator::Visit(While* stmt) {
  DuplicateAST(stmt->condition);
  Expr* condition = resultExpr;
  DuplicateAST(stmt->branch);
  resultStmt = new While(condition, resultStmt);
}

void ASTDuplicator::Visit(Expr* expr) {
  //empty expression = undefined value
  resultExpr = new Expr();
}

void ASTDuplicator::Visit(Assign* expr) {
  DuplicateAST(expr->value);
  resultExpr = new Assign(expr->name, resultExpr);
}

void ASTDuplicator::Visit(Binary* expr) {
  DuplicateAST(expr->lhs);
  Expr* lhs = resultExpr;

  DuplicateAST(expr->rhs);

  resultExpr = new Binary(lhs, expr->op, resultExpr);
}

void ASTDuplicator::Visit(Function* expr) {
  DuplicateAST(expr->block);
  resultExpr = new Function(expr->varList, dynamic_cast<Block*>(resultStmt));
}

void ASTDuplicator::Visit(Grouping* expr) {
  DuplicateAST(expr->inner);
  resultExpr = new Grouping(resultExpr);
}

void ASTDuplicator::Visit(Logical* expr) {
  DuplicateAST(expr->lhs);
  Expr* lhs = resultExpr;

  DuplicateAST(expr->rhs);

  resultExpr = new Logical(lhs, expr->op, resultExpr);
}

void ASTDuplicator::Visit(Unary* expr) {
  DuplicateAST(expr->rhs);
  resultExpr = new Unary(expr->op, resultExpr);
}

void ASTDuplicator::Visit(Value* expr) {
  resultExpr = new Value(expr->value);
}

void ASTDuplicator::Visit(Variable* expr) {
  resultExpr = new Variable(expr->name);
}
