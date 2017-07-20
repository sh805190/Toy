#pragma once

class Expr;
class Array;
class Assign;
class Binary;
class Class;
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
  virtual void Visit(Class*) = 0;
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
  Expr() = default;
  Expr(int ln) { line = ln; }
  virtual void Accept(ExprVisitor* visitor) {
    visitor->Visit(this);
  }
  int line = -1;
};

class Array: public Expr {
public:
  Array(int ln, std::vector<Expr*> exprVector) {
this->line = ln;
    this->exprVector = exprVector;
  }

  void Accept(ExprVisitor* visitor) override {
    visitor->Visit(this);
  }

  std::vector<Expr*> exprVector;
};

class Assign: public Expr {
public:
  Assign(int ln, Expr* target, Expr* value) {
this->line = ln;
    this->target = target;
    this->value = value;
  }

  void Accept(ExprVisitor* visitor) override {
    visitor->Visit(this);
  }

  Expr* target;
  Expr* value;
};

class Binary: public Expr {
public:
  Binary(int ln, Expr* lhs, Token op, Expr* rhs) {
this->line = ln;
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

class Class: public Expr {
public:
  Class(int ln, Block* block) {
this->line = ln;
    this->block = block;
  }

  void Accept(ExprVisitor* visitor) override {
    visitor->Visit(this);
  }

  Block* block;
};

class Function: public Expr {
public:
  Function(int ln, std::vector<std::string> parameterVector, Block* block) {
this->line = ln;
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
  Grouping(int ln, Expr* inner) {
this->line = ln;
    this->inner = inner;
  }

  void Accept(ExprVisitor* visitor) override {
    visitor->Visit(this);
  }

  Expr* inner;
};

class Index: public Expr {
public:
  Index(int ln, Expr* array, Expr* index) {
this->line = ln;
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
  Invocation(int ln, Expr* expr, std::vector<Expr*> exprVector) {
this->line = ln;
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
  Logical(int ln, Expr* lhs, Token op, Expr* rhs) {
this->line = ln;
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
  Unary(int ln, Token op, Expr* rhs) {
this->line = ln;
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
  Value(int ln, Literal* value) {
this->line = ln;
    this->value = value;
  }

  void Accept(ExprVisitor* visitor) override {
    visitor->Visit(this);
  }

  Literal* value;
};

class Variable: public Expr {
public:
  Variable(int ln, Token name) {
this->line = ln;
    this->name = name;
  }

  void Accept(ExprVisitor* visitor) override {
    visitor->Visit(this);
  }

  Token name;
};
