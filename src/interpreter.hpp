#pragma once

#include "expr.hpp"
#include "stmt.hpp"

#include "environment.hpp"
#include "literal.hpp"

class Interpreter: public ExprVisitor, public StmtVisitor {
public:
  void Execute(Stmt*);
  void Evaluate(Expr*);

  //visit methods
  void Visit(Stmt*) override;
  void Visit(Expression*) override;
  void Visit(Var*) override;

  void Visit(Expr*) override;
  void Visit(Assign*) override;
  void Visit(Binary*) override;
  void Visit(Grouping*) override;
  void Visit(Logical*) override;
  void Visit(Unary*) override;
  void Visit(Value*) override;
  void Visit(Variable*) override;

private:
  //helpers
  bool IsEqual(Literal, Literal);
  void CheckOperandsAreNumbers(Token op, Literal, Literal);
  bool IsTruthy(Literal);
  Literal Dereference(Literal);

  //members
  Environment environment;
  Literal result;
};
