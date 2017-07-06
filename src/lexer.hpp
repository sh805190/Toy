#pragma once

#include "literal.hpp"
#include "token_type.hpp"
#include "token.hpp"

#include <list>
#include <map>
#include <string>

class Lexer {
public:
  Lexer() = delete;
  Lexer(std::string source);

  std::list<Token> GetTokenList();

private:
  std::map<std::string, TokenType> keywords = {
    {"and",       AND},
    {"break",     BREAK},
    {"case",      CASE},
    {"class",     CLASS},
    {"continue",  CONTINUE},
    {"default",   DEFAULT},
    {"else",      ELSE},
    {"false",     FALSE},
    {"for",       FOR},
    {"function",  FUNCTION},
    {"goto",      GOTO},
    {"if",        IF},
    {"module",    MODULE},
    {"or",        OR},
    {"return",    RETURN},
    {"switch",    SWITCH},
    {"true",      TRUE},
    {"undefined", UNDEFINED},
    {"use",       USE},
    {"var",       VAR},
    {"while",     WHILE}
  };

  const std::string source;
  std::list<Token> tokenList;
  int current = 0;
  int start = 0;
  int line = 1;

  void ScanToken();

  //helpers
  void AddToken(TokenType type);
  void AddToken(TokenType type, Literal literal);
  char Advance();
  void EatComments();
  bool IsAlpha(char c);
  bool IsAtEnd();
  bool IsDigit(char c);
  bool Match(char expected);
  void ScanIdentifier();
  void ScanNumber();
  void ScanString();
};