#pragma once

#include "expr.hpp"
#include "stmt.hpp"

//warning: one use only
class ASTDuplicator: public ExprVisitor, public StmtVisitor {
public:
  Expr* DuplicateAST(Expr*);
  Stmt* DuplicateAST(Stmt*);

  //visit methods
  void Visit(Stmt*) override;
  void Visit(Block*) override;
  void Visit(Break*) override;
  void Visit(Continue*) override;
  void Visit(Expression*) override;
  void Visit(If*) override;
  void Visit(Return*) override;
  void Visit(Var*) override;
  void Visit(While*) override;

  void Visit(Expr*) override;
  void Visit(Assign*) override;
  void Visit(Binary*) override;
  void Visit(Function*) override;
  void Visit(Grouping*) override;
  void Visit(Logical*) override;
  void Visit(Unary*) override;
  void Visit(Value*) override;
  void Visit(Variable*) override;

private:
  Expr* resultExpr = nullptr;
  Stmt* resultStmt = nullptr;
};
