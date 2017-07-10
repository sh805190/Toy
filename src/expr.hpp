#pragma once

class Expr;
class Binary;
class Grouping;
class Logical;
class Unary;
class Value;

#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"

#include <list>

class ExprVisitor {
public:
  virtual void Visit(Expr*) = 0;
  virtual void Visit(Binary*) = 0;
  virtual void Visit(Grouping*) = 0;
  virtual void Visit(Logical*) = 0;
  virtual void Visit(Unary*) = 0;
  virtual void Visit(Value*) = 0;
};

class Expr {
public:
  virtual void Accept(ExprVisitor* visitor) {
    visitor->Visit(this);
  }
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
