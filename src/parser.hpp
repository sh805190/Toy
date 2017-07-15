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
  Stmt* ScanBreak();
  Stmt* ScanClass();
  Stmt* ScanContinue();
  Stmt* ScanFor();
  Stmt* ScanIf();
  Stmt* ScanReturn();
  Stmt* ScanModule();
  Stmt* ScanUse();
  Stmt* ScanVar();
  Stmt* ScanWhile();
  
  Expr* ScanExpression();

  //expr rules
  Expr* ScanAssignment();
  Expr* ScanLogical();
  Expr* ScanComparison();
  Expr* ScanTerm();
  Expr* ScanFactor();
  Expr* ScanUnary();
  Expr* ScanOperator();
  Expr* ScanPrimary();

  //other types
  Stmt* ScanBlock();
  Expr* ScanFunction();
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
  bool skipSemicolon = false;

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
