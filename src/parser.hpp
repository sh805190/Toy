#pragma once

#include "stmt.hpp"
#include "token.hpp"

#include <vector>

class Parser {
public:
  Parser() = delete;
  Parser(std::vector<Token> tokens);

  std::list<Stmt*> GetStatements();

private:
  //rules
  Stmt* ScanStatement();

  //stmt types
  Stmt* ScanClass();
  Stmt* ScanIf();
  Stmt* ScanReturn();
  Stmt* ScanUse();
  Stmt* ScanVar();
  Stmt* ScanWhile();
  Stmt* ScanBreak();
  Stmt* ScanContinue();
  Stmt* ScanModule();
  
  Expr* ScanExpression();

  //expr types
  Expr* ScanArray();
  Expr* ScanAssign();
  Expr* ScanBinary();
  Expr* ScanFunction();
  Expr* ScanGrouping();
  Expr* ScanIdentifier();
  Expr* ScanLiteral();
  Expr* ScanReference();
  Expr* ScanUnary();

  //other types
  Expr* ScanBlock();
  Expr* ScanSpecial();

  //helpers
  Token Advance();
  void Consume(Token token, std::string errmsg);
  bool IsAtEnd();
  bool Match(Token);
  void Synchronize();

  std::vector<Token> tokens;
  int current = 0;

  class ParserError {
  public:
    ParserError() = delete;
    ParserError(int l, std::string msg): line(l), errmsg(msg) {}
    int GetLine() { return line; }
    std::string GetErrMsg() { return errmsg; }
  private:
    const int line;
    std::string errmsg;
  };
};
