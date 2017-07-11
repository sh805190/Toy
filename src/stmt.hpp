#pragma once

class Stmt;
class Expression;
class Var;

#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"

#include <list>

class StmtVisitor {
public:
  virtual void Visit(Stmt*) = 0;
  virtual void Visit(Expression*) = 0;
  virtual void Visit(Var*) = 0;
};

class Stmt {
public:
  virtual void Accept(StmtVisitor* visitor) {
    visitor->Visit(this);
  }
};

class Expression: public Stmt {
public:
  Expression(Expr* expr) {
    this->expr = expr;
  }

  void Accept(StmtVisitor* visitor) override {
    visitor->Visit(this);
  }

  Expr* expr;
};

class Var: public Stmt {
public:
  Var(Token name, Expr* initializer) {
    this->name = name;
    this->initializer = initializer;
  }

  void Accept(StmtVisitor* visitor) override {
    visitor->Visit(this);
  }

  Token name;
  Expr* initializer;
};
