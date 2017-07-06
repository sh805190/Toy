#pragma once

#include "token_type.hpp"

#include "literal.hpp"

#include <string>

//token in the AST tree
class Token {
public:
  Token() = delete;
  Token(TokenType type, std::string lexeme, Literal literal, int line);

  TokenType GetType();
  std::string GetLexeme();
  Literal GetLiteral();
  int GetLine();

  //debugging
  std::string ToString();

private:
  TokenType type;
  std::string lexeme;
  Literal literal;
  const int line;
};