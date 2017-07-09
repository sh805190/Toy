#pragma once

#include "expr.hpp"
#include "token_type.hpp"

class TokenTypeGetter: public ExprVisitor {
public:
  void Visit(Expr* expr) {
    type = UNDEFINED;
  }

  void Visit(Binary* expr) {
    type = expr->op.GetType();
  }

  void Visit(Grouping* expr) {
    type = LEFT_PAREN;
  }

  void Visit(Unary* expr) {
    type = expr->op.GetType();
  }

  void Visit(Value* expr) {
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

  TokenType GetType() {
    return type;
  }

private:
  TokenType type;
};
