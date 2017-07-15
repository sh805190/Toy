#pragma once

class Expr;
class Assign;
class Binary;
class Function;
class Grouping;
class Invocation;
class Logical;
class Unary;
class Value;
class Variable;

#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"

#include <list>
#include <string>

class ExprVisitor {
public:
  virtual void Visit(Expr*) = 0;
  virtual void Visit(Assign*) = 0;
  virtual void Visit(Binary*) = 0;
  virtual void Visit(Function*) = 0;
  virtual void Visit(Grouping*) = 0;
  virtual void Visit(Invocation*) = 0;
  virtual void Visit(Logical*) = 0;
  virtual void Visit(Unary*) = 0;
  virtual void Visit(Value*) = 0;
  virtual void Visit(Variable*) = 0;
};

class Expr {
public:
  virtual void Accept(ExprVisitor* visitor) {
    visitor->Visit(this);
  }
};

class Assign: public Expr {
public:
  Assign(Token name, Expr* value) {
    this->name = name;
    this->value = value;
  }

  void Accept(ExprVisitor* visitor) override {
    visitor->Visit(this);
  }

  Token name;
  Expr* value;
};

class Binary: public Expr {
public:
  Binary(Expr* lhs, Token op, Expr* rhs) {
    this->lhs = lhs;
    this->op = op;
    this->rhs = rhs;
  }

  void Accept(ExprVisitor* visitor) override {
    visitor->Visit(this);
  }

  Expr* lhs;
  Token op;
  Expr* rhs;
};

class Function: public Expr {
public:
  Function(std::list<std::string> varList, Block* block) {
    this->varList = varList;
    this->block = block;
  }

  void Accept(ExprVisitor* visitor) override {
    visitor->Visit(this);
  }

  std::list<std::string> varList;
  Block* block;
};

class Grouping: public Expr {
public:
  Grouping(Expr* inner) {
    this->inner = inner;
  }

  void Accept(ExprVisitor* visitor) override {
    visitor->Visit(this);
  }

  Expr* inner;
};

class Invocation: public Expr {
public:
  Invocation(Expr* expr, std::list<Expr*> exprList) {
    this->expr = expr;
    this->exprList = exprList;
  }

  void Accept(ExprVisitor* visitor) override {
    visitor->Visit(this);
  }

  Expr* expr;
  std::list<Expr*> exprList;
};

class Logical: public Expr {
public:
  Logical(Expr* lhs, Token op, Expr* rhs) {
    this->lhs = lhs;
    this->op = op;
    this->rhs = rhs;
  }

  void Accept(ExprVisitor* visitor) override {
    visitor->Visit(this);
  }

  Expr* lhs;
  Token op;
  Expr* rhs;
};

class Unary: public Expr {
public:
  Unary(Token op, Expr* rhs) {
    this->op = op;
    this->rhs = rhs;
  }

  void Accept(ExprVisitor* visitor) override {
    visitor->Visit(this);
  }

  Token op;
  Expr* rhs;
};

class Value: public Expr {
public:
  Value(Literal value) {
    this->value = value;
  }

  void Accept(ExprVisitor* visitor) override {
    visitor->Visit(this);
  }

  Literal value;
};

class Variable: public Expr {
public:
  Variable(Token name) {
    this->name = name;
  }

  void Accept(ExprVisitor* visitor) override {
    visitor->Visit(this);
  }

  Token name;
};
