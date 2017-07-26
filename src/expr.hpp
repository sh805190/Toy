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
  Expr();
  Expr(int);
  virtual ~Expr();
  virtual void Accept(ExprVisitor* visitor);

  int line = -1;
  static int count;
};

class Array: public Expr {
public:
  Array(int ln, std::vector<Expr*> exprVector);
  virtual ~Array();
  void Accept(ExprVisitor* visitor) override;

  std::vector<Expr*> exprVector;
};

class Assign: public Expr {
public:
  Assign(int ln, Expr* target, Expr* value);
  virtual ~Assign();
  void Accept(ExprVisitor* visitor) override;

  Expr* target;
  Expr* value;
};

class Binary: public Expr {
public:
  Binary(int ln, Expr* lhs, Token op, Expr* rhs);
  virtual ~Binary();
  void Accept(ExprVisitor* visitor) override;

  Expr* lhs;
  Token op;
  Expr* rhs;
};

class Class: public Expr {
public:
  Class(int ln, Block* block);
  virtual ~Class();
  void Accept(ExprVisitor* visitor) override;

  Block* block;
};

class Function: public Expr {
public:
  Function(int ln, std::vector<std::string> parameterVector, Block* block);
  virtual ~Function();
  void Accept(ExprVisitor* visitor) override;

  std::vector<std::string> parameterVector;
  Block* block;
};

class Grouping: public Expr {
public:
  Grouping(int ln, Expr* inner);
  virtual ~Grouping();
  void Accept(ExprVisitor* visitor) override;

  Expr* inner;
};

class Index: public Expr {
public:
  Index(int ln, Expr* array, Expr* index);
  virtual ~Index();
  void Accept(ExprVisitor* visitor) override;

  Expr* array;
  Expr* index;
};

class Invocation: public Expr {
public:
  Invocation(int ln, Expr* expr, std::vector<Expr*> exprVector);
  virtual ~Invocation();
  void Accept(ExprVisitor* visitor) override;

  Expr* expr;
  std::vector<Expr*> exprVector;
};

class Logical: public Expr {
public:
  Logical(int ln, Expr* lhs, Token op, Expr* rhs);
  virtual ~Logical();
  void Accept(ExprVisitor* visitor) override;

  Expr* lhs;
  Token op;
  Expr* rhs;
};

class Unary: public Expr {
public:
  Unary(int ln, Token op, Expr* rhs);
  virtual ~Unary();
  void Accept(ExprVisitor* visitor) override;

  Token op;
  Expr* rhs;
};

class Value: public Expr {
public:
  Value(int ln, Literal* value);
  virtual ~Value();
  void Accept(ExprVisitor* visitor) override;

  Literal* value;
};

class Variable: public Expr {
public:
  Variable(int ln, Token name);
  virtual ~Variable();
  void Accept(ExprVisitor* visitor) override;

  Token name;
};
