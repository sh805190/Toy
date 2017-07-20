#include "parser.hpp"

#include "error_handler.hpp"
#include "expr_visitors.hpp"
#include "stmt_visitors.hpp"

Parser::Parser(std::vector<Token> t): tokenVector(t) {
  //EMPTY
}

Parser::~Parser() {
  tokenVector.clear();
}

std::vector<Stmt*> Parser::GetStmtVector() {
  statementVector.clear();

  while(!IsAtEnd()) {
    try {
      Stmt* stmt = ScanStatement();
      if (stmt) {
        statementVector.push_back(stmt);
      }
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
  Token &tok = tokenVector[current]; //work while pointing at the current token
  Stmt* ret = nullptr;

  switch(tok.GetType()) {
    //allow empty statements
    case SEMICOLON:
      //EMPTY
    break;

    //all types
    case BREAK:
      ret = ScanBreak();
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

    case LEFT_BRACE:
      ret = ScanBlock();
      skipSemicolon = true;
    break;

    //delegate
    default:
      ret = new Expression(tok.GetLine(), ScanExpression());
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
  Token &tok = Advance(); //skip break keyword
  return new Break(tok.GetLine());
}

Stmt* Parser::ScanContinue() {
  Token &tok = Advance(); //skip continue keyword
  return new Continue(tok.GetLine());
}

Stmt* Parser::ScanFor() {
  Token& tok = Advance(); //skip for keyword
 
  //heading
  Consume(LEFT_PAREN, "Expected '(' after for keyword");

  //initializer
  Stmt* initializer = nullptr;
  if (tokenVector[current].GetType() == VAR) {
    initializer = ScanVar();
  }
  else if (tokenVector[current].GetType() != SEMICOLON) {
    initializer = new Expression(tokenVector[current].GetLine(), ScanExpression());
  }

  Consume(SEMICOLON, "Expected ';' after for initializer");

  //condition
  Expr* condition = nullptr;
  if (tokenVector[current].GetType() != SEMICOLON) {
    condition = ScanExpression();
  }
  else {
    condition = new Value(tokenVector[current].GetLine(), new lBoolean(true)); //empty condition means forever
  }

  Consume(SEMICOLON, "Expected ';' after for condition");

  //incrementer
  Expr* increment = nullptr;
  if (tokenVector[current].GetType() != RIGHT_PAREN) {
    increment = ScanExpression();
  }

  Consume(RIGHT_PAREN, "Expected ')' after for increment");

  //get the block (ensures that the body of the for statement is a block object
  Stmt* block = nullptr;
  if (tokenVector[current].GetType() == LEFT_BRACE) {
    block = ScanBlock();
  }
  else {
    block = new Block(tokenVector[current].GetLine(), {ScanStatement()});
  }

  //store the body inside another block (prevent bugs with naming conflicts)
  block = new Block(block->line, {block});

  //piece it all together
  std::vector<Stmt*> stmtVector;

  if (initializer) {
    stmtVector.push_back(initializer);
  }
  if (increment) { //append to the block before inserting into the While
    dynamic_cast<Block*>(block)->stmtVector.push_back(new Expression(increment->line, increment));
  }
  stmtVector.push_back(new While(tok.GetLine(), condition, block));

  //one of those
  skipSemicolon = true;

  //finally, return the full, new block object
  return new Block(tok.GetLine(), stmtVector);
}

Stmt* Parser::ScanIf() {
  Token& tok = Advance(); //skip if keyword

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
    thenBranch = new Block(tokenVector[current].GetLine(), {ScanStatement()});
  }

  //else
  Stmt* elseBranch = nullptr;
  if (Match(ELSE)) {
    if (tokenVector[current].GetType() == LEFT_BRACE) {
      elseBranch = ScanBlock();
    }
    else {
      elseBranch = new Block(tokenVector[current].GetLine(), {ScanStatement()});
    }
  }

  //one of those
  skipSemicolon = true;

  //finally
  return new If(tok.GetLine(), condition, thenBranch, elseBranch);
}

Stmt* Parser::ScanModule() {
  //TODO
  throw ParserError(tokenVector[current].GetLine(), std::string() + "'" + tokenVector[current].GetLexeme() + "' not yet implemented");
}

Stmt* Parser::ScanReturn() {
  Token& tok = Advance(); //skip return keyword
  if (tokenVector[current].GetType() != SEMICOLON) {
    return new Return(tok.GetLine(), ScanExpression());
  }
  else {
    return new Return(tok.GetLine(), nullptr);
  }
}

Stmt* Parser::ScanUse() {
  //TODO
  throw ParserError(tokenVector[current].GetLine(), std::string() + "'" + tokenVector[current].GetLexeme() + "' not yet implemented");
}

Stmt* Parser::ScanVar() {
  Token& tok = Advance(); //skip var keyword
  Token name = Advance();
  Expr* initializer = nullptr;

  if (Match(EQUAL)) {
    initializer = ScanExpression();
  }

  return new Var(tok.GetLine(), name, initializer);
}

Stmt* Parser::ScanWhile() {
  Token& tok = Advance(); //skip while keyword

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
    branch = new Block(tokenVector[current].GetLine(), {ScanStatement()});
  }

  //one of those
  skipSemicolon = true;

  //finally
  return new While(tok.GetLine(), condition, branch);
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

    if (typeGetter.GetType() != IDENTIFIER && typeGetter.GetType() != INDEX && typeGetter.GetType() != STAR && typeGetter.GetType() != DOT) {
      throw ParserError(expr->line, std::string() + "Invalid assignment target");
    }

    //get the variable token
    Token op = tokenVector[current-1];
    Expr* rhs = ScanExpression();

    //simple
    expr = new Assign(op.GetLine(), expr, rhs);
  }

  return expr;
}

Expr* Parser::ScanLogical() {
  Expr* expr = ScanComparison();

  if (Match(AND) || Match(OR)) {
    Token op = tokenVector[current-1];
    Expr* rhs = ScanComparison();
    expr = new Logical(op.GetLine(), expr, op, rhs);
  }

  return expr;
}

Expr* Parser::ScanComparison() {
  Expr* expr = ScanTerm();

  if (Match(EQUAL_EQUAL) || Match(BANG_EQUAL) || Match(LESS) || Match(LESS_EQUAL) || Match(GREATER) || Match(GREATER_EQUAL)) {
    Token op = tokenVector[current-1];
    Expr* rhs = ScanTerm();
    expr = new Binary(op.GetLine(), expr, op, rhs);
  }

  return expr;
}

Expr* Parser::ScanTerm() {
  Expr* expr = ScanFactor();

  if (Match(PLUS) || Match(MINUS)) {
    Token op = tokenVector[current-1];
    Expr* rhs = ScanFactor();
    expr = new Binary(op.GetLine(), expr, op, rhs);
  }

  return expr;
}

Expr* Parser::ScanFactor() {
  Expr* expr = ScanUnary();

  if (Match(STAR) || Match(SLASH)) {
    Token op = tokenVector[current-1];

    //account for reference notation screwing with arithmetic
    if (op.GetType() == STAR && dynamic_cast<lNumber*>(op.GetLiteral())->number != 1) {
      throw ParserError(op.GetLine(), "Too many characters in '" + op.GetLexeme() + "' (did you mean to use a reference?)");
    }

    Expr* rhs = ScanUnary();
    expr = new Binary(op.GetLine(), expr, op, rhs);
  }

  return expr;
}

Expr* Parser::ScanUnary() {
  //negation
  if (Match(MINUS) || Match(BANG)) {
    Token op = tokenVector[current-1];
    Expr* rhs = ScanUnary();
    return new Unary(op.GetLine(), op, rhs);
  }

  //references
  if (Match(AMPERSAND) || Match(STAR)) {
    Token op = tokenVector[current-1];
    Expr* expr = ScanOperator();

    TokenTypeGetter typeGetter;
    expr->Accept(&typeGetter);

    if (typeGetter.GetType() != IDENTIFIER) {
      throw ParserError(op.GetLine(), std::string() + "Operand of '" + op.GetLexeme() + "' must be a variable");
    }

    return new Unary(op.GetLine(), op, expr);
  }

  return ScanOperator();
}

Expr* Parser::ScanOperator() {
  //operation = actions preformed on a primary/binding
  Expr* expr = ScanBinding();

  //chained arrays/functions need a loop
  for (;;) {
    //array access
    if (Match(LEFT_BRACKET)) {
      Token op = tokenVector[current-1];
      Expr* index = ScanExpression();
      Consume(RIGHT_BRACKET, "Expected ']' after array index");

      expr = new Index(op.GetLine(), expr, index);
    }

    //function/class call
    else if (Match(LEFT_PAREN)) {
      Token op = tokenVector[current-1];
      std::vector<Expr*> exprVector;
      if (!Match(RIGHT_PAREN)) {
        while(!IsAtEnd()) {
          exprVector.push_back(ScanExpression());
          if (Match(RIGHT_PAREN)) break;
          Consume(COMMA, "Expected ',' between arguments");
        }
      }
      expr = new Invocation(op.GetLine(), expr, exprVector);
    }

    //done
    else {
      break;
    }
  }

  return expr;
}

Expr* Parser::ScanBinding() {
  Expr* expr = ScanPrimary();

  //accessing a member of an object
  if (Match(DOT)) {
    Token op = tokenVector[current-1];
    Expr* rhs = ScanBinding();
    expr = new Binary(op.GetLine(), expr, op, rhs);
  }

/*    //accessing a member of a reference
    else if (Match(MINUS_GREATER)) {
      //syntactic sugar
      Token op = tokenVector[current-1];
      expr = new Unary(Token(STAR, "*", Literal(1.0), op.GetLine()), expr);
      Expr* rhs = ScanOperator();
      expr = new Binary(expr, Token(DOT, ".", Literal(), op.GetLine()), rhs);
    }
*/

  return expr;
}

Expr* Parser::ScanPrimary() {
  int line = tokenVector[current].GetLine();

  if (Match(FALSE)) return new Value(line, new lBoolean(false));
  if (Match(TRUE)) return new Value(line, new lBoolean(true));

  if (Match(UNDEFINED)) return new Expr(line);

  if (Match(NUMBER) || Match(STRING)) return new Value(line, tokenVector[current-1].GetLiteral()->Copy());

  //array creation
  if (Match(LEFT_BRACKET)) {
    std::vector<Expr*> exprVector;
    while(!IsAtEnd()) {
      exprVector.push_back(ScanExpression());
      if (Match(RIGHT_BRACKET)) break;
      Consume(COMMA, "Expected ',' between array elements");
    }
    return new Array(line, exprVector);
  }

  if (Match(FUNCTION)) {
    return ScanFunction();
  }

  if (Match(CLASS)) {
    return ScanClass();
  }

  //grouping
  if (Match(LEFT_PAREN)) {
    Expr* expr = ScanExpression();
    Consume(RIGHT_PAREN, "Expected ')' after expression");
    return new Grouping(line, expr);
  }

  if (Match(IDENTIFIER)) {
    return new Variable(line, tokenVector[current-1]);
  }

  //unknown token
  throw(ParserError(line, std::string() + "Unexpected symbol '" + tokenVector[current-1].GetLexeme() + "'"));
}

//other types
Stmt* Parser::ScanBlock() {
  Consume(LEFT_BRACE, "Exprected '{' at beginning of a block");

  //error handling
  int ln = tokenVector[current-1].GetLine();

  std::vector<Stmt*> stmtVector;

  while(!IsAtEnd() && tokenVector[current].GetType() != RIGHT_BRACE) {
    //copy the main scanning loop
    try {
      stmtVector.push_back(ScanStatement());
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
  return new Block(ln, stmtVector);
}

Expr* Parser::ScanClass() {
  StmtTypeGetter typeGetter;
  int ln = tokenVector[current-1].GetLine(); //from the class keyword

  //Get the block
  Block* block = dynamic_cast<Block*>(ScanBlock());

  //ensure all statements are var declarations
  for (Stmt* ptr : block->stmtVector) {
    ptr->Accept(&typeGetter);
    if (typeGetter.GetType() != VAR) {
      throw ParserError(-1, "Only variable declarations and definitions are allowed inside class definitions");
    }
  }

  return new Class(ln, block);
}

Expr* Parser::ScanFunction() {
  int ln = tokenVector[current-1].GetLine(); //from the function keyword

  std::vector<std::string> formalParameters;

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

  return new Function(ln, formalParameters, dynamic_cast<Block*>(block));
}

Expr* Parser::ScanSpecial() {
  //TODO
  throw ParserError(tokenVector[current].GetLine(), std::string() + "'" + tokenVector[current].GetLexeme() + "' not yet implemented");
}

//helpers
Token& Parser::Advance() {
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
