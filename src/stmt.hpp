#pragma once

class Stmt;
class Expression;
class Block;
class If;
class Var;
class While;

#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"

#include <list>

class StmtVisitor {
public:
  virtual void Visit(Stmt*) = 0;
  virtual void Visit(Expression*) = 0;
  virtual void Visit(Block*) = 0;
  virtual void Visit(If*) = 0;
  virtual void Visit(Var*) = 0;
  virtual void Visit(While*) = 0;
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

class Block: public Stmt {
public:
  Block(std::list<Stmt*> stmtList) {
    this->stmtList = stmtList;
  }

  void Accept(StmtVisitor* visitor) override {
    visitor->Visit(this);
  }

  std::list<Stmt*> stmtList;
};

class If: public Stmt {
public:
  If(Expr* condition, Stmt* thenBranch, Stmt* elseBranch) {
    this->condition = condition;
    this->thenBranch = thenBranch;
    this->elseBranch = elseBranch;
  }

  void Accept(StmtVisitor* visitor) override {
    visitor->Visit(this);
  }

  Expr* condition;
  Stmt* thenBranch;
  Stmt* elseBranch;
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

class While: public Stmt {
public:
  While(Expr* condition, Stmt* branch) {
    this->condition = condition;
    this->branch = branch;
  }

  void Accept(StmtVisitor* visitor) override {
    visitor->Visit(this);
  }

  Expr* condition;
  Stmt* branch;
};
