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

#include <string>
#include <vector>

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
  Stmt() = default;
  Stmt(int ln) { line = ln; }
  virtual void Accept(StmtVisitor* visitor) {
    visitor->Visit(this);
  }
  int line = -1;
};

class Block: public Stmt {
public:
  Block(int ln, std::vector<Stmt*> stmtVector) {
    this->line = ln;
    this->stmtVector = stmtVector;
  }

  void Accept(StmtVisitor* visitor) override {
    visitor->Visit(this);
  }

  std::vector<Stmt*> stmtVector;
};

class Break: public Stmt {
public:
  Break(int ln) {
    this->line = ln;
  }

  void Accept(StmtVisitor* visitor) override {
    visitor->Visit(this);
  }

 };

class Continue: public Stmt {
public:
  Continue(int ln) {
    this->line = ln;
  }

  void Accept(StmtVisitor* visitor) override {
    visitor->Visit(this);
  }

 };

class Expression: public Stmt {
public:
  Expression(int ln, Expr* expr) {
    this->line = ln;
    this->expr = expr;
  }

  void Accept(StmtVisitor* visitor) override {
    visitor->Visit(this);
  }

  Expr* expr;
};

class If: public Stmt {
public:
  If(int ln, Expr* condition, Stmt* thenBranch, Stmt* elseBranch) {
    this->line = ln;
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
  Return(int ln, Expr* result) {
    this->line = ln;
    this->result = result;
  }

  void Accept(StmtVisitor* visitor) override {
    visitor->Visit(this);
  }

  Expr* result;
};

class Var: public Stmt {
public:
  Var(int ln, Token name, Expr* initializer) {
    this->line = ln;
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
  While(int ln, Expr* condition, Stmt* branch) {
    this->line = ln;
    this->condition = condition;
    this->branch = branch;
  }

  void Accept(StmtVisitor* visitor) override {
    visitor->Visit(this);
  }

  Expr* condition;
  Stmt* branch;
};
