#pragma once

#include "expr.hpp"
#include "token_type.hpp"

class TokenTypeGetter: public ExprVisitor {
public:
  void Visit(Expr* expr) override {
    type = UNDEFINED;
  }

  void Visit(Array* expr) override {
    type = ARRAY;
  }

  void Visit(Assign* expr) override {
    type = EQUAL;
  }

  void Visit(Binary* expr) override {
    type = expr->op.GetType();
  }

  void Visit(Class* expr) override {
    type = CLASS;
  }

  void Visit(Function* expr) override {
    type = FUNCTION;
  }

  void Visit(Grouping* expr) override {
    type = LEFT_PAREN;
  }

  void Visit(Index* expr) override {
    type = INDEX;
  }

  void Visit(Invocation* expr) override {
    type = INVOCATION;
  }

  void Visit(Logical* expr) override {
    type = expr->op.GetType();
  }

  void Visit(Unary* expr) override {
    type = expr->op.GetType();
  }

  void Visit(Value* expr) override {
    switch(expr->value.GetType()) {
      case Literal::Type::BOOLEAN:
        type = BOOLEAN;
      break;
      case Literal::Type::NUMBER:
        type = NUMBER;
      break;
      case Literal::Type::STRING:
        type = STRING;
      break;
    }
  }

  void Visit(Variable* expr) override {
    type = IDENTIFIER;
  }

  TokenType GetType() {
    return type;
  }

private:
  TokenType type;
};

class TokenGetter: public ExprVisitor {
public:
  void Visit(Expr* expr) override {
    token = Token(UNDEFINED, "Na", Literal(), -1);
  }

  void Visit(Array* expr) override {
    token = Token(ARRAY, "Na", Literal(), -2);
  }

  void Visit(Assign* expr) override {
    token = Token(EQUAL, "Na", Literal(), -3);
  }

  void Visit(Binary* expr) override {
    token = expr->op;
  }

  void Visit(Class* expr) override {
    token = Token(CLASS, "Na", Literal(), -4);
  }

  void Visit(Function* expr) override {
    token = Token(FUNCTION, "Na", Literal(), -6);
  }

  void Visit(Grouping* expr) override {
    token = Token(LEFT_PAREN, "Na", Literal(), -7);
  }

  void Visit(Index* expr) override {
    token = Token(INDEX, "Na", Literal(), -8);
  }

  void Visit(Invocation* expr) override {
    token = Token(INVOCATION, "Na", Literal(), -9);
  }

  void Visit(Logical* expr) override {
    token = expr->op;
  }

  void Visit(Unary* expr) override {
    token = expr->op;
  }

  void Visit(Value* expr) override {
    token = Token(VALUE, "Na", Literal(), -10);
  }

  void Visit(Variable* expr) override {
    token = expr->name;
  }

  Token GetToken() {
    return token;
  }

private:
  Token token;
};
