#ifndef __StmtGUARD
#define __StmtGUARD

#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"

#include <list>

class Stmt;
class Expression;

class StmtVisitor {
public:
  virtual void Visit(Stmt*) = 0;
  virtual void Visit(Expression*) = 0;
};

class Stmt {
public:
  virtual void Accept(StmtVisitor* visitor) {
    visitor->Visit(this);
  }
};

class Expression: public Stmt {
public:
  Expression(Expr expr) {
    this->expr = expr;
  }

  void Accept(StmtVisitor* visitor) override {
    visitor->Visit(this);
  }

  Expr expr;
};

#endif
