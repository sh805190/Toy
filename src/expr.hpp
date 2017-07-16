#pragma once

class Expr;
class Array;
class Assign;
class Binary;
class Function;
class Grouping;
class Index;
class Invocation;
class Logical;
class Unary;
class Value;
class Variable;

#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"

#include <string>
#include <vector>

class ExprVisitor {
public:
  virtual void Visit(Expr*) = 0;
  virtual void Visit(Array*) = 0;
  virtual void Visit(Assign*) = 0;
  virtual void Visit(Binary*) = 0;
  virtual void Visit(Function*) = 0;
  virtual void Visit(Grouping*) = 0;
  virtual void Visit(Index*) = 0;
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

class Array: public Expr {
public:
  Array(std::vector<Expr*> exprVector) {
    this->exprVector = exprVector;
  }

  void Accept(ExprVisitor* visitor) override {
    visitor->Visit(this);
  }

  std::vector<Expr*> exprVector;
};

class Assign: public Expr {
public:
  Assign(Token op, Expr* target, Expr* value) {
    this->op = op;
    this->target = target;
    this->value = value;
  }

  void Accept(ExprVisitor* visitor) override {
    visitor->Visit(this);
  }

  Token op;
  Expr* target;
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
  Function(std::vector<std::string> parameterVector, Block* block) {
    this->parameterVector = parameterVector;
    this->block = block;
  }

  void Accept(ExprVisitor* visitor) override {
    visitor->Visit(this);
  }

  std::vector<std::string> parameterVector;
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

class Index: public Expr {
public:
  Index(Expr* array, Expr* index) {
    this->array = array;
    this->index = index;
  }

  void Accept(ExprVisitor* visitor) override {
    visitor->Visit(this);
  }

  Expr* array;
  Expr* index;
};

class Invocation: public Expr {
public:
  Invocation(Expr* expr, std::vector<Expr*> exprVector) {
    this->expr = expr;
    this->exprVector = exprVector;
  }

  void Accept(ExprVisitor* visitor) override {
    visitor->Visit(this);
  }

  Expr* expr;
  std::vector<Expr*> exprVector;
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
