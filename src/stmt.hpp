#pragma once

class Stmt;
class Block;
class Break;
class Continue;
class Expression;
class If;
class Return;
class Var;
class While;

#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"

#include <list>
#include <string>

class StmtVisitor {
public:
  virtual void Visit(Stmt*) = 0;
  virtual void Visit(Block*) = 0;
  virtual void Visit(Break*) = 0;
  virtual void Visit(Continue*) = 0;
  virtual void Visit(Expression*) = 0;
  virtual void Visit(If*) = 0;
  virtual void Visit(Return*) = 0;
  virtual void Visit(Var*) = 0;
  virtual void Visit(While*) = 0;
};

class Stmt {
public:
  virtual void Accept(StmtVisitor* visitor) {
    visitor->Visit(this);
  }
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

class Break: public Stmt {
public:
  Break(int line) {
    this->line = line;
  }

  void Accept(StmtVisitor* visitor) override {
    visitor->Visit(this);
  }

  int line;
};

class Continue: public Stmt {
public:
  Continue(int line) {
    this->line = line;
  }

  void Accept(StmtVisitor* visitor) override {
    visitor->Visit(this);
  }

  int line;
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

class Return: public Stmt {
public:
  Return(int line, Expr* result) {
    this->line = line;
    this->result = result;
  }

  void Accept(StmtVisitor* visitor) override {
    visitor->Visit(this);
  }

  int line;
  Expr* result;
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
