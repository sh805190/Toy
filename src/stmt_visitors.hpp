#pragma once

#include "stmt.hpp"
#include "token_type.hpp"

class StmtTypeGetter : public StmtVisitor {
public:
  TokenType GetType() {
    return type;
  }

  void Visit(Stmt* stmt) {
    type = UNDEFINED;
  }

  void Visit(Block* stmt) {
    type = LEFT_BRACE;
  }

  void Visit(Break* stmt) {
    type = BREAK;
  }

  void Visit(Continue* stmt) {
    type = CONTINUE;
  }

  void Visit(Expression* stmt) {
    type = SEMICOLON;
  }

  void Visit(If* stmt) {
    type = IF;
  }

  void Visit(Return* stmt) {
    type = RETURN;
  }

  void Visit(Use* stmt) {
    type = USE;
  }

  void Visit(Var* stmt) {
    type = VAR;
  }

  void Visit(While* stmt) {
    type = WHILE;
  }

private:
  TokenType type;
};
