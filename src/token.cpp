#include "token.hpp"

Token::Token() {
  type = UNDEFINED;
  lexeme = "";
  literal = nullptr;
  line = -1;
}

Token::Token(const Token& rhs) {
  *this = rhs;
}

Token::Token(Token&& rhs) {
  *this = std::move(rhs);
}

Token::Token(TokenType argType) {
  type = argType;
  lexeme = "";
  literal = nullptr;
  line = -1;
}

Token::Token(TokenType argType, std::string argLexeme, Literal* argLiteral, int argLine) {
  type = argType;
  lexeme = argLexeme;
  literal = argLiteral;
  line = argLine;
}

Token& Token::operator=(const Token& rhs) {
  this->type = rhs.type;
  this->lexeme = rhs.lexeme;
  this->literal = rhs.literal->Copy();
  this->line = rhs.line;
  return *this;
}

Token& Token::operator=(Token&& rhs) {
  this->type = rhs.type;
  this->lexeme = rhs.lexeme;
  this->literal = rhs.literal;
  this->line = rhs.line;

  rhs.type = UNDEFINED;
  rhs.lexeme = "";
  rhs.literal = nullptr;
  rhs.line = -1;

  return *this;
}

TokenType Token::GetType() {
  return type;
}

std::string Token::GetLexeme() {
  return lexeme;
}

Literal* Token::GetLiteral() {
  return literal;
}

int Token::GetLine() {
  return line;
}

std::string Token::ToString() {
  std::string output = std::string() + "Token " + std::to_string(int(type)) + " '" + lexeme + "' ";
  if (literal) {
    output += literal->ToString();
    output += " ";
  }
  else {
    output +=  "nullptr ";
  }
  output += std::string() + "line " + std::to_string(line) + ";";
  return output;
}
