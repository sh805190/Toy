#pragma once

#include "expr.hpp"
#include "stmt.hpp"

class ASTReader: public ExprVisitor, public StmtVisitor  {
public:
  void Print(Stmt*);
  void Print(Expr*);

  //visit methods
  void Visit(Stmt*) override;
  void Visit(Block*) override;
  void Visit(Break*) override;
  void Visit(Continue*) override;
  void Visit(Expression*) override;
  void Visit(If*) override;
  void Visit(Return*) override;
  void Visit(Use*) override;
  void Visit(Var*) override;
  void Visit(While*) override;

  void Visit(Expr*) override;
  void Visit(Array*) override;
  void Visit(Assign*) override;
  void Visit(Binary*) override;
  void Visit(Class*) override;
  void Visit(Function*) override;
  void Visit(Grouping*) override;
  void Visit(Index*) override;
  void Visit(Invocation*) override;
  void Visit(Logical*) override;
  void Visit(Unary*) override;
  void Visit(Value*) override;
  void Visit(Variable*) override;
};

