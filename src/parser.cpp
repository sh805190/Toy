#include "parser.hpp"

#include "error_handler.hpp"
#include "expr_visitors.hpp"

Parser::Parser(std::vector<Token> t): tokenVector(t) {
  //EMPTY
}

std::vector<Stmt*> Parser::GetStmtVector() {
  statementVector.clear();

  while(!IsAtEnd()) {
    try {
      statementVector.push_back(ScanStatement());
    }
    catch (ParserError pe) {
      ErrorHandler::Error(pe.GetLine(), pe.GetErrMsg());
      Advance();
      Synchronize();
    }
  }
  return statementVector;
}

//rules
Stmt* Parser::ScanStatement() {
  Token tok = tokenVector[current]; //work while pointing at the current token

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
      Advance();
      throw ParserError(tok.GetLine(), "Block scoping is not allowed");

    //delegate
    default: {
      Expr* ret = ScanExpression();
      Consume(SEMICOLON, "Expected ';' at end of statement");
      return new Expression(ret);
    }
  }
}

//stmt types
Stmt* Parser::ScanClass() {
  //TODO
  throw ParserError(tokenVector[current].GetLine(), std::string() + "'" + tokenVector[current].GetLexeme() + "' not yet implemented");
}

Stmt* Parser::ScanIf() {
  //TODO
  throw ParserError(tokenVector[current].GetLine(), std::string() + "'" + tokenVector[current].GetLexeme() + "' not yet implemented");
}

Stmt* Parser::ScanReturn() {
  //TODO
  throw ParserError(tokenVector[current].GetLine(), std::string() + "'" + tokenVector[current].GetLexeme() + "' not yet implemented");
}

Stmt* Parser::ScanUse() {
  //TODO
  throw ParserError(tokenVector[current].GetLine(), std::string() + "'" + tokenVector[current].GetLexeme() + "' not yet implemented");
}

Stmt* Parser::ScanVar() {
  //TODO
  throw ParserError(tokenVector[current].GetLine(), std::string() + "'" + tokenVector[current].GetLexeme() + "' not yet implemented");
}

Stmt* Parser::ScanWhile() {
  //TODO
  throw ParserError(tokenVector[current].GetLine(), std::string() + "'" + tokenVector[current].GetLexeme() + "' not yet implemented");
}

Stmt* Parser::ScanBreak() {
  //TODO
  throw ParserError(tokenVector[current].GetLine(), std::string() + "'" + tokenVector[current].GetLexeme() + "' not yet implemented");
}

Stmt* Parser::ScanContinue() {
  //TODO
  throw ParserError(tokenVector[current].GetLine(), std::string() + "'" + tokenVector[current].GetLexeme() + "' not yet implemented");
}

Stmt* Parser::ScanModule() {
  //TODO
  throw ParserError(tokenVector[current].GetLine(), std::string() + "'" + tokenVector[current].GetLexeme() + "' not yet implemented");
}

Expr* Parser::ScanExpression() {
  return ScanAssignment();
}

//expr rules
Expr* Parser::ScanAssignment() {
  Expr* expr = ScanLogical();

  if (Match(EQUAL)) {
    //check the lhs expression type
    TokenTypeGetter getter;
    expr->Accept(&getter);
    if (getter.GetType() != IDENTIFIER) {
      throw ParserError(tokenVector[current-1].GetLine(), "invalid assignment target");
    }

    Token op = tokenVector[current-1];
    Expr* rhs = ScanAssignment();
    expr = new Binary(expr, op, rhs);
 }

  return expr;
}

Expr* Parser::ScanLogical() {
  Expr* expr = ScanComparison();

  if (Match(AND) || Match(OR)) {
    Token op = tokenVector[current-1];
    Expr* rhs = ScanComparison();
    expr = new Binary(expr, op, rhs);
  }

  return expr;
}

Expr* Parser::ScanComparison() {
  Expr* expr = ScanTerm();

  if (Match(EQUAL_EQUAL) || Match(BANG_EQUAL) || Match(LESS) || Match(LESS_EQUAL) || Match(GREATER) || Match(GREATER_EQUAL)) {
    Token op = tokenVector[current-1];
    Expr* rhs = ScanTerm();
    expr = new Binary(expr, op, rhs);
  }

  return expr;
}

Expr* Parser::ScanTerm() {
  Expr* expr = ScanFactor();

  if (Match(PLUS) || Match(MINUS)) {
    Token op = tokenVector[current-1];
    Expr* rhs = ScanFactor();
    expr = new Binary(expr, op, rhs);
  }

  return expr;
}

Expr* Parser::ScanFactor() {
  Expr* expr = ScanUnary();

  if (Match(STAR) || Match(SLASH)) {
    Token op = tokenVector[current-1];
    Expr* rhs = ScanUnary();
    expr = new Binary(expr, op, rhs);
  }

  return expr;
}

Expr* Parser::ScanUnary() {
  if (Match(MINUS) || Match(BANG)) {
    Token op = tokenVector[current-1];
    Expr* rhs = ScanUnary();
    return new Unary(op, rhs);
  }

  return ScanPrimary();
}

Expr* Parser::ScanPrimary() {
  if (Match(FALSE)) return new Value(false);
  if (Match(TRUE)) return new Value(true);

  if (Match(UNDEFINED)) return new Expr();

  if (Match(NUMBER) || Match(STRING)) return new Value(tokenVector[current-1].GetLiteral());

  if (Match(LEFT_PAREN)) {
    Expr* expr = ScanExpression();
    Consume(RIGHT_PAREN, "Expected ')' after expression");
    return new Grouping(expr);
  }

  Advance(); //skip this token
  throw(ParserError(tokenVector[current-1].GetLine(), std::string() + "Unexpected symbol '" + tokenVector[current-1].GetLexeme() + "'"));
}

//other types
Expr* Parser::ScanBlock() {
  //TODO
  throw ParserError(tokenVector[current].GetLine(), std::string() + "'" + tokenVector[current].GetLexeme() + "' not yet implemented");
}

Expr* Parser::ScanSpecial() {
  //TODO
  throw ParserError(tokenVector[current].GetLine(), std::string() + "'" + tokenVector[current].GetLexeme() + "' not yet implemented");
}

//helpers
Token Parser::Advance() {
  current++;
  return tokenVector[current - 1];
}

void Parser::Consume(Token token, std::string errmsg) {
  if (IsAtEnd() || !Match(token)) {
    ErrorHandler::Error(token.GetLine(), errmsg);
    Synchronize();
  }
}

bool Parser::IsAtEnd() {
  return (current >= tokenVector.size()) || (tokenVector[current].GetType() == END_OF_FILE);
}

bool Parser::Match(Token t) {
  if (!IsAtEnd() && tokenVector[current].GetType() == t.GetType()) {
    Advance();
    return true;
  }
  return false;
}

void Parser::Synchronize() {
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
