#pragma once

#include "expr.hpp"
#include "stmt.hpp"

#include "environment.hpp"
#include "literal.hpp"

class Interpreter: public ExprVisitor, public StmtVisitor {
public:
  Interpreter(Environment* parent = nullptr, Environment* forced = nullptr);
  ~Interpreter();

  void Execute(Stmt*);
  void Evaluate(Expr*);

  bool GetReturnCalled();
  Literal* SetResult(Literal* ptr);
  Literal* GetResult();

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

private:
  //helpers
  void CallFunction(int line, lFunction* func, std::vector<Literal*> literalVector, lObject* self = nullptr);
  void CreateObject(int line, lClass* prototype, std::vector<Literal*> literalVector);
  bool IsEqual(Literal* lhs, Literal* rhs);
  void CheckOperandsAreNumbers(Token op, Literal* lhs, Literal* rhs);
  bool IsTruthy(Literal*);
  Literal* Dereference(Literal*);

  //members
  Environment* environment = nullptr;
  Literal* result = nullptr;

  //block contexts
  bool breakCalled = false;
  bool continueCalled = false;
  bool returnCalled = false;
  int loopDepth = 0;
};
