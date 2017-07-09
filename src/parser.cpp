#include "parser.hpp"

#include "error_handler.hpp"

Parser::Parser(std::vector<Token> t): tokenVector(t) {
  //EMPTY
}

std::vector<Stmt*> Parser::GetStatements() {
  statementVector.clear();

  while(!IsAtEnd()) {
    try {
      statementVector.push_back(ScanStatement());
    }
    catch (ParserError pe) {
      ErrorHandler::Error(pe.GetLine(), pe.GetErrMsg());
      Synchronize();
    }
  }

  return statementVector;
}

//rules
Stmt* Parser::ScanStatement() {
  Token tok = Advance();

  switch(tok.GetType()) {
    //all types
    case CLASS: return ScanClass();
    case IF: return ScanIf();
    case RETURN: return ScanReturn();
    case USE: return ScanUse();
    case VAR: return ScanVar();
    case WHILE: return ScanWhile();
    case BREAK: return ScanBreak();
    case CONTINUE: return ScanContinue();
    case MODULE: return ScanModule();

    //error
    case LEFT_BRACE:
      throw ParserError(tok.GetLine(), "Block scoping is not allowed");

    //delegate
    default: {
      Expr* ret = ScanExpression();
      Consume(SEMICOLON, "Expected ';' after expression");
      return new Expression(ret);
    }
  }
}

//stmt types
Stmt* Parser::ScanClass() {
  //TODO
  throw ParserError(tokenVector[current-1].GetLine(), std::string() + "'" + tokenVector[current-1].GetLexeme() + "' not yet implemented");
}

Stmt* Parser::ScanIf() {
  //TODO
  throw ParserError(tokenVector[current-1].GetLine(), std::string() + "'" + tokenVector[current-1].GetLexeme() + "' not yet implemented");
}

Stmt* Parser::ScanReturn() {
  //TODO
  throw ParserError(tokenVector[current-1].GetLine(), std::string() + "'" + tokenVector[current-1].GetLexeme() + "' not yet implemented");
}

Stmt* Parser::ScanUse() {
  //TODO
  throw ParserError(tokenVector[current-1].GetLine(), std::string() + "'" + tokenVector[current-1].GetLexeme() + "' not yet implemented");
}

Stmt* Parser::ScanVar() {
  //TODO
  throw ParserError(tokenVector[current-1].GetLine(), std::string() + "'" + tokenVector[current-1].GetLexeme() + "' not yet implemented");
}

Stmt* Parser::ScanWhile() {
  //TODO
  throw ParserError(tokenVector[current-1].GetLine(), std::string() + "'" + tokenVector[current-1].GetLexeme() + "' not yet implemented");
}

Stmt* Parser::ScanBreak() {
  //TODO
  throw ParserError(tokenVector[current-1].GetLine(), std::string() + "'" + tokenVector[current-1].GetLexeme() + "' not yet implemented");
}

Stmt* Parser::ScanContinue() {
  //TODO
  throw ParserError(tokenVector[current-1].GetLine(), std::string() + "'" + tokenVector[current-1].GetLexeme() + "' not yet implemented");
}

Stmt* Parser::ScanModule() {
  //TODO
  throw ParserError(tokenVector[current-1].GetLine(), std::string() + "'" + tokenVector[current-1].GetLexeme() + "' not yet implemented");
}

Expr* Parser::ScanExpression() {
  //TODO
  throw ParserError(tokenVector[current-1].GetLine(), std::string() + "'" + tokenVector[current-1].GetLexeme() + "' not yet implemented");
}

//expr types
Expr* Parser::ScanArray() {
  //TODO
  throw ParserError(tokenVector[current-1].GetLine(), std::string() + "'" + tokenVector[current-1].GetLexeme() + "' not yet implemented");
}

Expr* Parser::ScanAssign() {
  //TODO
  throw ParserError(tokenVector[current-1].GetLine(), std::string() + "'" + tokenVector[current-1].GetLexeme() + "' not yet implemented");
}

Expr* Parser::ScanBinary() {
  //TODO
  throw ParserError(tokenVector[current-1].GetLine(), std::string() + "'" + tokenVector[current-1].GetLexeme() + "' not yet implemented");
}

Expr* Parser::ScanFunction() {
  //TODO
  throw ParserError(tokenVector[current-1].GetLine(), std::string() + "'" + tokenVector[current-1].GetLexeme() + "' not yet implemented");
}

Expr* Parser::ScanGrouping() {
  //TODO
  throw ParserError(tokenVector[current-1].GetLine(), std::string() + "'" + tokenVector[current-1].GetLexeme() + "' not yet implemented");
}

Expr* Parser::ScanIdentifier() {
  //TODO
  throw ParserError(tokenVector[current-1].GetLine(), std::string() + "'" + tokenVector[current-1].GetLexeme() + "' not yet implemented");
}

Expr* Parser::ScanLiteral() {
  //TODO
  throw ParserError(tokenVector[current-1].GetLine(), std::string() + "'" + tokenVector[current-1].GetLexeme() + "' not yet implemented");
}

Expr* Parser::ScanReference() {
  //TODO
  throw ParserError(tokenVector[current-1].GetLine(), std::string() + "'" + tokenVector[current-1].GetLexeme() + "' not yet implemented");
}

Expr* Parser::ScanUnary() {
  //TODO
  throw ParserError(tokenVector[current-1].GetLine(), std::string() + "'" + tokenVector[current-1].GetLexeme() + "' not yet implemented");
}

//other types
Expr* Parser::ScanBlock() {
  //TODO
  throw ParserError(tokenVector[current-1].GetLine(), std::string() + "'" + tokenVector[current-1].GetLexeme() + "' not yet implemented");
}

Expr* Parser::ScanSpecial() {
  //TODO
  throw ParserError(tokenVector[current-1].GetLine(), std::string() + "'" + tokenVector[current-1].GetLexeme() + "' not yet implemented");
}

//helpers
Token Parser::Advance() {
  current++;
  return tokenVector[current - 1];
}

void Parser::Consume(Token token, std::string errmsg) {
  if (!Match(token)) {
    ErrorHandler::Error(token.GetLine(), errmsg);
    Synchronize();
  }
}

bool Parser::IsAtEnd() {
  return current >= tokenVector.size();
}

bool Parser::Match(Token t) {
  if (tokenVector[current].GetType() == t.GetType()) {
    Advance();
    return true;
  }
  return false;
}

void Parser::Synchronize() {
  Advance();

  while(!IsAtEnd()) {
    if (tokenVector[current - 1].GetType() == SEMICOLON) return;
    switch(tokenVector[current].GetType()) {
      case BREAK:
      case CASE:
      case CLASS:
      case CONTINUE:
      case DEFAULT:
      case FOR:
      case GOTO:
      case IF:
      case MODULE:
      case RETURN:
      case SWITCH:
      case USE:
      case VAR:
      case WHILE:
        return;
    }
    Advance();
  }
}
