#pragma once

#include "expr.hpp"
#include "token_type.hpp"

class TokenTypeGetter: public ExprVisitor {
public:
  void Visit(Expr* expr) override {
    type = UNDEFINED;
  }

  void Visit(Assign* expr) override {
    type = EQUAL;
  }

  void Visit(Binary* expr) override {
    type = expr->op.GetType();
  }

  void Visit(Function* expr) override {
    type = FUNCTION;
  }

  void Visit(Grouping* expr) override {
    type = LEFT_PAREN;
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
    token = Token(END_OF_FILE, "Na", Literal(), -1);
  }

  void Visit(Assign* expr) override {
    token = expr->name;
  }

  void Visit(Binary* expr) override {
    token = expr->op;
  }

  void Visit(Function* expr) override {
    token = Token(END_OF_FILE, "Na", Literal(), -1);
  }

  void Visit(Grouping* expr) override {
    token = Token(END_OF_FILE, "Na", Literal(), -2);
  }

  void Visit(Logical* expr) override {
    token = expr->op;
  }

  void Visit(Unary* expr) override {
    token = expr->op;
  }

  void Visit(Value* expr) override {
    token = Token(END_OF_FILE, "Na", Literal(), -3);
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
