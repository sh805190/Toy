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

  bool SetFlag(std::string, bool);
  bool GetFlag(std::string);

private:
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

  //helpers
  bool IsEqual(Literal* lhs, Literal* rhs);
  void CheckOperandsAreNumbers(Token op, Literal* lhs, Literal* rhs);
  bool IsTruthy(Literal*);
  Literal* Dereference(Literal*);

  void CheckVersion(int line, Literal*);
  void LoadModule(std::string);
  bool LoadBuiltinModule(std::string);

  //members
  Environment* environment = nullptr;
  Literal* result = nullptr;
  std::map<std::string, bool> useFlag;

  //block contexts
  bool breakCalled = false;
  bool continueCalled = false;
  bool returnCalled = false;
  int loopDepth = 0;
};
