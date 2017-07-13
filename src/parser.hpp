#pragma once

#include "stmt.hpp"
#include "token.hpp"

#include <vector>

class Parser {
public:
  Parser() = delete;
  Parser(std::vector<Token> tokens);

  std::vector<Stmt*> GetStmtVector();

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

  //expr rules
  Expr* ScanAssignment();
  Expr* ScanLogical();
  Expr* ScanComparison();
  Expr* ScanTerm();
  Expr* ScanFactor();
  Expr* ScanUnary();
  Expr* ScanPrimary();

  //other types
  Stmt* ScanBlock();
  Expr* ScanSpecial();

  //helpers
  Token Advance();
  void Consume(Token token, std::string errmsg);
  bool IsAtEnd();
  bool Match(Token);
  void Synchronize();
 
  std::vector<Stmt*> statementVector;
  std::vector<Token> tokenVector;
  int current = 0;
  bool ignoreSemicolon = false;

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
