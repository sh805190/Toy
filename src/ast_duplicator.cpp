#include "ast_duplicator.hpp"

#include "literal.hpp"
#include "runtime_error.hpp"

Expr* ASTDuplicator::DuplicateAST(Expr* expr) {
  if (expr == nullptr) {
    return resultExpr = nullptr;
  }

  expr->Accept(this);
  return resultExpr;
}

Stmt* ASTDuplicator::DuplicateAST(Stmt* stmt) {
  if (stmt == nullptr) {
    return resultStmt = nullptr;
  }

  stmt->Accept(this);
  return resultStmt;
}

void ASTDuplicator::Visit(Stmt* stmt) {
  //Nothing can be done
  throw RuntimeError(-1, "Can't duplicate empty statement");
}

void ASTDuplicator::Visit(Block* stmt) {
  std::vector<Stmt*> stmtVector;
  for (Stmt* it : stmt->stmtVector) {
    DuplicateAST(it);
    stmtVector.push_back(resultStmt);
  }
  resultStmt = new Block(stmt->line, stmtVector);
}

void ASTDuplicator::Visit(Break* stmt) {
  resultStmt = new Break(stmt->line);
}

void ASTDuplicator::Visit(Continue* stmt) {
  resultStmt = new Continue(stmt->line);
}

void ASTDuplicator::Visit(Expression* stmt) {
  DuplicateAST(stmt->expr);
  resultStmt = new Expression(stmt->line, resultExpr);
}

void ASTDuplicator::Visit(If* stmt) {
  DuplicateAST(stmt->condition);
  Expr* condition = resultExpr;

  DuplicateAST(stmt->thenBranch);
  Stmt* thenBranch = resultStmt;

  DuplicateAST(stmt->elseBranch);
  Stmt* elseBranch = resultStmt;

  resultStmt = new If(stmt->line, condition, thenBranch, elseBranch);
}

void ASTDuplicator::Visit(Return* stmt) {
  DuplicateAST(stmt->result);
  resultStmt = new Return(stmt->line, resultExpr);
}

void ASTDuplicator::Visit(Use* stmt) {
  resultStmt = new Use(stmt->line, stmt->command);
}

void ASTDuplicator::Visit(Var* stmt) {
  DuplicateAST(stmt->initializer);
  resultStmt = new Var(stmt->line, stmt->name, resultExpr);
}

void ASTDuplicator::Visit(While* stmt) {
  DuplicateAST(stmt->condition);
  Expr* condition = resultExpr;
  DuplicateAST(stmt->branch);
  resultStmt = new While(stmt->line, condition, resultStmt);
}

void ASTDuplicator::Visit(Expr* expr) {
  //empty expression = undefined value
  resultExpr = new Expr(expr->line);
}

void ASTDuplicator::Visit(Array* expr) {
  std::vector<Expr*> exprVector;
  for (Expr* ptr : expr->exprVector) {
    DuplicateAST(ptr);
    exprVector.push_back(resultExpr);
  }
  resultExpr = new Array(expr->line, exprVector);
}

void ASTDuplicator::Visit(Assign* expr) { 
  DuplicateAST(expr->target);
  Expr* target = resultExpr;
  DuplicateAST(expr->value);
  Expr* value = resultExpr;
  resultExpr = new Assign(expr->line, target, value);
}

void ASTDuplicator::Visit(Binary* expr) {
  DuplicateAST(expr->lhs);
  Expr* lhs = resultExpr;

  DuplicateAST(expr->rhs);

  resultExpr = new Binary(expr->line, lhs, expr->op, resultExpr);
}

void ASTDuplicator::Visit(Class* expr) {
  DuplicateAST(expr->block);
  resultExpr = new Class(expr->line, static_cast<Block*>(resultStmt));
}

void ASTDuplicator::Visit(Function* expr) {
  DuplicateAST(expr->block);
  resultExpr = new Function(expr->line, expr->parameterVector, static_cast<Block*>(resultStmt));
}

void ASTDuplicator::Visit(Grouping* expr) {
  DuplicateAST(expr->inner);
  resultExpr = new Grouping(expr->line, resultExpr);
}

void ASTDuplicator::Visit(Index* expr) {
  DuplicateAST(expr->array);
  Expr* array = resultExpr;
  DuplicateAST(expr->index);
  Expr* index = resultExpr;
  resultExpr = new Index(expr->line, array, index);
}

void ASTDuplicator::Visit(Invocation* expr) {
  //function
  DuplicateAST(expr->expr);
  Expr* func = resultExpr;

  std::vector<Expr*> exprVector;
  for (Expr* ptr : expr->exprVector) {
    DuplicateAST(ptr);
    exprVector.push_back(resultExpr);
  }

  resultExpr = new Invocation(expr->line, func, exprVector);
}

void ASTDuplicator::Visit(Logical* expr) {
  DuplicateAST(expr->lhs);
  Expr* lhs = resultExpr;

  DuplicateAST(expr->rhs);

  resultExpr = new Logical(expr->line, lhs, expr->op, resultExpr);
}

void ASTDuplicator::Visit(Unary* expr) {
  DuplicateAST(expr->rhs);
  resultExpr = new Unary(expr->line, expr->op, resultExpr);
}

void ASTDuplicator::Visit(Value* expr) {
  resultExpr = new Value(expr->line, expr->value->Copy());
}

void ASTDuplicator::Visit(Variable* expr) {
  resultExpr = new Variable(expr->line, expr->name);
}
