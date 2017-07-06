#include "token.hpp"

Token::Token(TokenType argType, std::string argLexeme, Literal argLiteral, int argLine):
  type(argType), lexeme(argLexeme), literal(argLiteral), line(argLine)
{
  //EMPTY
}

TokenType Token::GetType() {
  return type;
}

std::string Token::GetLexeme() {
  return lexeme;
}

Literal Token::GetLiteral() {
  return literal;
}

int Token::GetLine() {
  return line;
}

std::string Token::ToString() {
  std::string ret;
  ret += "Token " + std::to_string(int(type)) + " " + lexeme + " " + literal.ToString() + " line " + std::to_string(line) + ";";
  return ret;
}
