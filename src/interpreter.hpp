#pragma once

#include "expr.hpp"
#include "stmt.hpp"

#include "environment.hpp"
#include "literal.hpp"

class Interpreter: public ExprVisitor, public StmtVisitor {
public:
  Interpreter(Environment* env = nullptr);

  void Execute(Stmt*);
  void Evaluate(Expr*);

  bool GetReturnCalled();
  Literal GetResult();

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
  Environment* environment = nullptr;
  Literal result;

  //block contexts
  bool breakCalled = false;
  bool continueCalled = false;
  bool returnCalled = false;
  int loopDepth = 0;
};
