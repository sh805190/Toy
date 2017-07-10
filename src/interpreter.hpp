#pragma once

#include "expr.hpp"
#include "stmt.hpp"

#include "literal.hpp"

class Interpreter: public ExprVisitor, public StmtVisitor {
public:
  void Execute(Stmt*);
  void Evaluate(Expr*);

  //visit methods
  void Visit(Stmt*) override;
  void Visit(Expression*) override;
  void Visit(Expr*) override;
  void Visit(Binary*) override;
  void Visit(Grouping*) override;
  void Visit(Logical*) override;
  void Visit(Unary*) override;
  void Visit(Value*) override;

private:
  //helpers
  bool IsEqual(Literal, Literal);
  void CheckOperandsAreNumbers(Token op, Literal, Literal);
  bool IsTruthy(Literal);

  //members
  Literal result;

  //error handling
  class InterpreterError {
  public:
    InterpreterError() = delete;
    InterpreterError(int l, std::string msg): line(l), errmsg(msg) {}
    int GetLine() { return line; }
    std::string GetErrMsg() { return errmsg; }
  private:
    const int line;
    std::string errmsg;
  };
};
