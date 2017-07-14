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
  Stmt* ret = nullptr;

  switch(tok.GetType()) {
    //all types
    case BREAK:
      ret = ScanBreak();
    break;

    case CLASS:
      ret = ScanClass();
    break;

    case CONTINUE:
      ret = ScanContinue();
    break;

    case FOR:
      ret = ScanFor();
    break;

    case IF:
      ret = ScanIf();
    break;

    case MODULE:
      ret = ScanModule();
    break;

    case RETURN:
      ret = ScanReturn();
    break;

    case USE:
      ret = ScanUse();
    break;

    case VAR:
      ret = ScanVar();
    break;

    case WHILE:
      ret = ScanWhile();
    break;

    //error
    case LEFT_BRACE:
      throw ParserError(tok.GetLine(), "Block scoping is not allowed");

    //delegate
    default:
      ret = new Expression(ScanExpression());
  }

  //finally
  if (!skipSemicolon) {
    Consume(SEMICOLON, "Expected ';' at end of statement");
  }

  skipSemicolon = false;

  return ret;
}

//stmt types
Stmt* Parser::ScanBreak() {
  Advance(); //skip break keyword
  return new Break(tokenVector[current-1].GetLine());
}

Stmt* Parser::ScanClass() {
  //TODO
  throw ParserError(tokenVector[current].GetLine(), std::string() + "'" + tokenVector[current].GetLexeme() + "' not yet implemented");
}

Stmt* Parser::ScanContinue() {
  Advance(); //skip continue keyword
  return new Continue(tokenVector[current-1].GetLine());
}

Stmt* Parser::ScanFor() {
  Advance(); //skip for keyword
 
  //heading
  Consume(LEFT_PAREN, "Expected '(' after for keyword");

  //initializer
  Stmt* initializer = nullptr;
  if (tokenVector[current].GetType() == VAR) {
    initializer = ScanVar();
  }
  else if (tokenVector[current].GetType() != SEMICOLON) {
    initializer = new Expression(ScanExpression());
  }

  Consume(SEMICOLON, "Expected ';' after for initializer");

  //condition
  Expr* condition = nullptr;
  if (tokenVector[current].GetType() != SEMICOLON) {
    condition = ScanExpression();
  }
  else {
    condition = new Value(true); //empty condition means forever
  }

  Consume(SEMICOLON, "Expected ';' after for condition");

  //incrementer
  Expr* increment = nullptr;
  if (tokenVector[current].GetType() != RIGHT_PAREN) {
    increment = ScanExpression();
  }

  Consume(RIGHT_PAREN, "Expected ';' after for increment");

  //get the block (ensures that the body of the for statement is a block object
  Stmt* block = nullptr;
  if (tokenVector[current].GetType() == LEFT_BRACE) {
    block = ScanBlock();
  }
  else {
    std::list<Stmt*> stmtList;
    stmtList.push_back(ScanStatement());
    block = new Block(stmtList);
  }

  //store the body inside another block (prevent bugs with naming conflicts)
  block = new Block({block});

  //piece it all together
  std::list<Stmt*> stmtList;

  if (initializer) {
    stmtList.push_back(initializer);
  }
  if (increment) {
    dynamic_cast<Block*>(block)->stmtList.push_back(new Expression(increment));
  }
  stmtList.push_back(new While(condition, block));

  //one of those
  skipSemicolon = true;

  //finally, return the full, new block object
  return new Block(stmtList);
}

Stmt* Parser::ScanIf() {
  Advance(); //skip if keyword

  //conditional
  Consume(LEFT_PAREN, "Expected '(' after if keyword");
  Expr* condition = ScanExpression();
  Consume(RIGHT_PAREN, "Expected ')' after if condition");

  //then
  Stmt* thenBranch = nullptr;
  if (tokenVector[current].GetType() == LEFT_BRACE) {
    thenBranch = ScanBlock();
  }
  else {
    thenBranch = ScanStatement();
  }

  //else
  Stmt* elseBranch = nullptr;
  if (Match(ELSE)) {
    if (tokenVector[current].GetType() == LEFT_BRACE) {
      elseBranch = ScanBlock();
    }
    else {
      elseBranch = ScanStatement();
    }
  }

  //one of those
  skipSemicolon = true;

  //finally
  return new If(condition, thenBranch, elseBranch);
}

Stmt* Parser::ScanModule() {
  //TODO
  throw ParserError(tokenVector[current].GetLine(), std::string() + "'" + tokenVector[current].GetLexeme() + "' not yet implemented");
}

Stmt* Parser::ScanReturn() {
  Advance(); //skip return keyword
  if (tokenVector[current].GetType() != SEMICOLON) {
    return new Return(tokenVector[current-1].GetLine(), ScanExpression());
  }
  else {
    return new Return(tokenVector[current-1].GetLine(), nullptr);
  }
}

Stmt* Parser::ScanUse() {
  //TODO
  throw ParserError(tokenVector[current].GetLine(), std::string() + "'" + tokenVector[current].GetLexeme() + "' not yet implemented");
}

Stmt* Parser::ScanVar() {
  Advance(); //skip var keyword
  Token name = Advance();
  Expr* initializer = nullptr;

  if (Match(EQUAL)) {
    initializer = ScanExpression();
  }

  return new Var(name, initializer);
}

Stmt* Parser::ScanWhile() {
  Advance(); //skip while keyword

  //conditional
  Consume(LEFT_PAREN, "Expected '(' after while keyword");
  Expr* condition = ScanExpression();
  Consume(RIGHT_PAREN, "Expected ')' after while condition");

  //then
  Stmt* branch = nullptr;
  if (tokenVector[current].GetType() == LEFT_BRACE) {
    branch = ScanBlock();
  }
  else {
    branch = ScanStatement();
  }

  //one of those
  skipSemicolon = true;

  //finally
  return new While(condition, branch);
}

Expr* Parser::ScanExpression() {
  return ScanAssignment();
}

//expr rules
Expr* Parser::ScanAssignment() {
  Expr* expr = ScanLogical();

  if (Match(EQUAL)) {
    //check the lhs expression type
    TokenTypeGetter typeGetter;
    expr->Accept(&typeGetter);
    if (typeGetter.GetType() != IDENTIFIER && typeGetter.GetType() != STAR) {
      throw ParserError(tokenVector[current-1].GetLine(), "Invalid assignment target");
    }

    //get the variable token
    TokenGetter tokenGetter;
    expr->Accept(&tokenGetter);

    Token op = tokenVector[current-1];
    Expr* rhs = ScanExpression();

    //simple variable
    if (typeGetter.GetType() == IDENTIFIER) {
      expr = new Assign(tokenGetter.GetToken(), rhs);
    }
    //altering a dereferenced variable
    else if (typeGetter.GetType() == STAR) {
      Unary* unary = dynamic_cast<Unary*>(expr); //unary object passed up
      Variable* variable = dynamic_cast<Variable*>(unary->rhs); //variable object it's pointing too

      //HACK: using tokenGetter here to keep track of how deep to dereference
      expr = new Assign(Token(REFERENCE, variable->name.GetLexeme(), tokenGetter.GetToken().GetLiteral(), variable->name.GetLine()), rhs);
    }
  }

  return expr;
}

Expr* Parser::ScanLogical() {
  Expr* expr = ScanComparison();

  if (Match(AND) || Match(OR)) {
    Token op = tokenVector[current-1];
    Expr* rhs = ScanComparison();
    expr = new Logical(expr, op, rhs);
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

    //account for reference notation screwing with arithmetic
    if (op.GetType() == STAR && op.GetLiteral().GetNumber() != 1) {
      throw ParserError(op.GetLine(), "Too many characters in '" + op.GetLexeme() + "' (did you mean to use a reference?)");
    }

    Expr* rhs = ScanUnary();
    expr = new Binary(expr, op, rhs);
  }

  return expr;
}

Expr* Parser::ScanUnary() {
  //negation
  if (Match(MINUS) || Match(BANG)) {
    Token op = tokenVector[current-1];
    Expr* rhs = ScanUnary();
    return new Unary(op, rhs);
  }

  //references
  if (Match(AMPERSAND) || Match(STAR)) {
    Token op = tokenVector[current-1];
    Expr* expr = ScanPrimary();

    TokenTypeGetter typeGetter;
    expr->Accept(&typeGetter);

    if (typeGetter.GetType() != IDENTIFIER) {
      throw ParserError(op.GetLine(), std::string() + "Operand of '" + op.GetLexeme() + "' must be a variable");
    }

    return new Unary(op, expr);
  }

  return ScanPrimary();
}

Expr* Parser::ScanPrimary() {
  if (Match(FALSE)) return new Value(false);
  if (Match(TRUE)) return new Value(true);

  if (Match(UNDEFINED)) return new Expr();

  if (Match(NUMBER) || Match(STRING)) return new Value(tokenVector[current-1].GetLiteral());

  if (Match(FUNCTION)) {
    return ScanFunction();
  }

  if (Match(LEFT_PAREN)) {
    Expr* expr = ScanExpression();
    Consume(RIGHT_PAREN, "Expected ')' after expression");
    return new Grouping(expr);
  }

  if (Match(IDENTIFIER)) {
    return new Variable(tokenVector[current-1]);
  }

  //unknown token
  throw(ParserError(tokenVector[current-1].GetLine(), std::string() + "Unexpected symbol '" + tokenVector[current-1].GetLexeme() + "'"));
}

//other types
Stmt* Parser::ScanBlock() {
  Consume(LEFT_BRACE, "Exprected '{' at beginning of a block");

  //error handling
  int ln = tokenVector[current-1].GetLine();

  std::list<Stmt*> stmtList;

  while(!IsAtEnd() && tokenVector[current].GetType() != RIGHT_BRACE) {
    //copy the main scanning loop
    try {
      stmtList.push_back(ScanStatement());
    }
    catch (ParserError pe) {
      ErrorHandler::Error(pe.GetLine(), pe.GetErrMsg());
      Advance();
      Synchronize();
    }
  }

  if (IsAtEnd()) {
    throw ParserError(ln, "No matching '}' for '{'");
  }

  Consume(RIGHT_BRACE, "Exprected '}' at end of a block");
  return new Block(stmtList);
}

Expr* Parser::ScanFunction() {
  std::list<std::string> formalParameters;

  Consume(LEFT_PAREN, "Expected '(' after function keyword");

  //parse formal parameters (overly engineered)
  if (tokenVector[current].GetType() != RIGHT_PAREN) {
    while(!IsAtEnd()) {
      Token t = Advance();
      if (t.GetType() != IDENTIFIER) {
        throw ParserError(t.GetLine(), std::string() + "Unexpected token '" + t.GetLexeme() + "' where formal parameter expected");
      }
      formalParameters.push_back(t.GetLexeme());
      if (Match(RIGHT_PAREN)) break;
      Consume(COMMA, "Expected ',' between formal parameters");
    }
  }
  else {
    Consume(RIGHT_PAREN, "Expected ')' after formal parameters");
  }

  //get the block
  Stmt* block = ScanBlock();

  return new Function(formalParameters, dynamic_cast<Block*>(block));
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
    ErrorHandler::Error(tokenVector[current].GetLine(), errmsg);
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
