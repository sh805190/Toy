#include "interpreter.hpp"

#include "ast_deleter.hpp"
#include "error_handler.hpp"
#include "runtime_error.hpp"

//TMP
#include <iostream>
#include <functional>

Interpreter::Interpreter(Environment* env) {
  environment = new Environment(env);
}

Interpreter::~Interpreter() {
  std::function<void(Environment*)> cleanup = [&cleanup](Environment* env) -> void {
    if (env != nullptr && env->GetParent() != nullptr) {
      cleanup(env->GetParent());
    }

    delete env;
  };

  cleanup(environment);
}

void Interpreter::Execute(Stmt* stmt) {
  //for block contexts
  breakCalled = false;
  continueCalled = false;
  returnCalled = false;

  try {
    result = Literal();
    stmt->Accept(this);

    //TMP
    std::cout << "INTERPRETER:" << result.ToString() << std::endl;
  }
  catch(RuntimeError re) {
    ErrorHandler::Error(re.GetLine(), re.GetErrMsg());
  }
}

void Interpreter::Evaluate(Expr* expr) {
  expr->Accept(this);
}

bool Interpreter::GetReturnCalled() {
  return returnCalled;
}

Literal Interpreter::GetResult() {
  return result;
}

void Interpreter::Visit(Stmt* stmt) {
  //should never happen
  throw RuntimeError(-1, "Empty statement in AST"); 
}

void Interpreter::Visit(Block* stmt) {
  environment = new Environment(environment);

  try {
    for (Stmt* ptr : stmt->stmtVector) {
      Execute(ptr);

      //stop block execution if...
      if (breakCalled || continueCalled || returnCalled) break;
    }
  }
  catch(RuntimeError& e) {
    //in case of emergencies, decrease the scope
    Environment* env = environment;
    environment = env->GetParent();
    delete env;

    //continue up the callstack
    throw(e);
  }

  //decrease scope
  Environment* env = environment;
  environment = env->GetParent();
  delete env;
}

void Interpreter::Visit(Break* stmt) {
  if (loopDepth <= 0) {
    throw RuntimeError(stmt->line, std::string() + "Break called with a loop depth of " + std::to_string(loopDepth));
  }

  breakCalled = true;
}

void Interpreter::Visit(Continue* stmt) {
  if (loopDepth <= 0) {
    throw RuntimeError(stmt->line, std::string() + "Continue called with a loop depth of " + std::to_string(loopDepth));
  }

  continueCalled = true;
}

void Interpreter::Visit(Expression* stmt) {
  Evaluate(stmt->expr);
}

void Interpreter::Visit(If* stmt) {
  //determine the result
  Evaluate(stmt->condition);

  if (IsTruthy(result)) {
    Execute(stmt->thenBranch);
  }

  else {
    if (stmt->elseBranch != nullptr) {
      Execute(stmt->elseBranch);
    }
  }
  //NOTE: block contexts continue outward
}

void Interpreter::Visit(Return* stmt) {
  result = Literal(); //explicitly undefined

  if (stmt->result) {
    Evaluate(stmt->result);
  }

  returnCalled = true;
}

void Interpreter::Visit(Var* stmt) {
  //define this variable
  if (stmt->initializer != nullptr) {
    Evaluate(stmt->initializer);
    environment->Define(stmt->name, result);
  } 
  else {
    //undefined literal
    environment->Define(stmt->name, Literal());
  }
}

void Interpreter::Visit(While* stmt) {
  loopDepth++; //diagnostics

  for (;;) {
    //block contexts
    if (breakCalled) {
      breakCalled = false;
      loopDepth--;
      break;
    }

    if (returnCalled) {
      loopDepth--;
      return; //don't reset the context flags
    }

    continueCalled = false;

    //if continuing
    Evaluate(stmt->condition);
    if (!IsTruthy(result)) {
      loopDepth--;
      break;
    }

    //execute the loop
    Execute(stmt->branch);
  }

  //reset these after a block
  breakCalled = false;
  continueCalled = false;
  returnCalled = false;
}

void Interpreter::Visit(Expr* expr) {
  //empty = undefined
  result = Literal();
}

void Interpreter::Visit(Assign* expr) {
  Evaluate(expr->value);
  if (expr->name.GetType() == IDENTIFIER) {
    environment->Assign(expr->name, result);
  }

  else if (expr->name.GetType() == REFERENCE) {
    std::string starLexeme;

    //HACK: get the number of dereference stars
    int starCount = expr->name.GetLiteral().GetNumber();

    Literal literal = environment->GetVar(expr->name); //base reference before dereferencing

    while(starCount > 0) {
      //display
      starLexeme += "*";

      //check that the literal type is a reference
      if (literal.GetType() != Literal::Type::REFERENCE) {
        throw RuntimeError(expr->name.GetLine(), std::string() + "Expression '" + starLexeme + expr->name.GetLexeme() + "' is not a variable");
      }

      //set the concrete reference via one level of dereference
      if (starCount == 1) {
        *(literal.GetReference()) = result;
        break;
      }

      //one level of dereference
      literal = *(literal.GetReference());

      starCount--;
    }
  }
}

void Interpreter::Visit(Binary* expr) {
  //evaluate each side
  Evaluate(expr->lhs);
  Literal lhs = result;
  Evaluate(expr->rhs);
  Literal rhs = result;

  switch(expr->op.GetType()) {
    //equality operators
    case EQUAL_EQUAL:
      result = IsEqual(lhs, rhs);
    break;

    case BANG_EQUAL:
      result = !IsEqual(lhs, rhs);
    break;

    //comparison operators
    case LESS:
      CheckOperandsAreNumbers(expr->op, lhs, rhs);
      result = lhs.GetNumber() < rhs.GetNumber();
    break;

    case LESS_EQUAL:
      CheckOperandsAreNumbers(expr->op, lhs, rhs);
      result = lhs.GetNumber() <= rhs.GetNumber();
    break;

    case GREATER:
      CheckOperandsAreNumbers(expr->op, lhs, rhs);
      result = lhs.GetNumber() > rhs.GetNumber();
    break;

    case GREATER_EQUAL:
      CheckOperandsAreNumbers(expr->op, lhs, rhs);
      result = lhs.GetNumber() >= rhs.GetNumber();
    break;

    //arithmatic operators
    case MINUS:
      CheckOperandsAreNumbers(expr->op, lhs, rhs);
      result = lhs.GetNumber() - rhs.GetNumber();
    break;

    case PLUS: //special case for the plus operator
      lhs = Dereference(lhs);
      rhs = Dereference(rhs);

      if (lhs.GetType() == Literal::Type::NUMBER && rhs.GetType() == Literal::Type::NUMBER) {
        result = lhs.GetNumber() + rhs.GetNumber();
      }
      else if (lhs.GetType() == Literal::Type::STRING && rhs.GetType() == Literal::Type::STRING) {
        result = lhs.GetString() + rhs.GetString();
      }
      else {
        throw RuntimeError(expr->op.GetLine(), std::string() + "Operands of '" + expr->op.GetLexeme() + "' must be both numbers or both strings");
      }
    break;

    case SLASH:
      CheckOperandsAreNumbers(expr->op, lhs, rhs);
      result = lhs.GetNumber() / rhs.GetNumber();
    break;

    case STAR:
      CheckOperandsAreNumbers(expr->op, lhs, rhs);
      result = lhs.GetNumber() * rhs.GetNumber();
    break;

    default:
      throw RuntimeError(expr->op.GetLine(), std::string() + "Unexpected binary operator '" + expr->op.GetLexeme() + "'");
  }
}

void Interpreter::Visit(Function* expr) {
  //TODO: make the function into a liteal
  result = Literal(expr->parameterVector, expr->block);
}

void Interpreter::Visit(Grouping* expr) {
  Evaluate(expr->inner);
}

void Interpreter::Visit(Invocation* expr) {
  //get the call requirements
  Evaluate(expr->expr);

  if (result.GetType() != Literal::Type::FUNCTION) {
    throw RuntimeError(-1, std::string() + "'" + result.ToString() + "' is not a function");
  }

  Literal func = result;

  std::vector<Literal> literalVector;
  for (Expr* ptr : expr->exprVector) {
    Evaluate(ptr);
    literalVector.push_back(result);
  }

  //finally
  CallFunction(func, literalVector);
}

void Interpreter::Visit(Logical* expr) {
  Literal lhs;
  Literal rhs;

  //evaluate the first side
  Evaluate(expr->lhs);

  switch(expr->op.GetType()) {
    //equality operators
    case AND:
      if (IsTruthy(result)) {
        Evaluate(expr->rhs);
      }
    break;

    case OR:
      if (IsTruthy(result)) {
        //EMPTY
      }
      else {
        Evaluate(expr->rhs);
      }
    break;

    default:
      throw RuntimeError(expr->op.GetLine(), std::string() + "Unexpected logical operator '" + expr->op.GetLexeme() + "'");
  }
}

void Interpreter::Visit(Unary* expr) {
  Evaluate(expr->rhs);

  switch(expr->op.GetType()) {
    case MINUS:
      if (result.GetType() == Literal::Type::NUMBER) {
        result = -result.GetNumber();
      }
      else {
        throw RuntimeError(expr->op.GetLine(), "Can only negate a number");
      }
    break;

    case BANG:
      result = !IsTruthy(result);
    break;

    case AMPERSAND:
      result = environment->GetRef(dynamic_cast<Variable*>(expr->rhs)->name);
    break;

    case STAR: {
      std::string starLexeme;
      int starCount = expr->op.GetLiteral().GetNumber();
      Literal literal = result;
      while(starCount > 0) {
        //display
        starLexeme += "*";

        //check that the literal type is a reference
        if (literal.GetType() != Literal::Type::REFERENCE) {
          Variable* variable = dynamic_cast<Variable*>(expr->rhs);
          throw RuntimeError(variable->name.GetLine(), std::string() + "Expression '" + starLexeme + variable->name.GetLexeme() + "' is not a variable");
        }

        //one level of dereference
        literal = *(literal.GetReference());

        starCount--;
      }
      result = literal;
    }
    break;
  }
}

void Interpreter::Visit(Value* expr) {
  result = expr->value;
}

void Interpreter::Visit(Variable* expr) {
  result = environment->GetVar(expr->name);
}

//helpers
void Interpreter::CallFunction(Literal func, std::vector<Literal> literalVector) {
  //build the environment
  std::vector<std::string> parameterVector = func.GetParameterVector();

  if (parameterVector.size() != literalVector.size()) {
    //TODO: much better error message
    throw RuntimeError(-1, std::string() + "Invalid number of arguments (expected " + std::to_string(parameterVector.size()) + ", received " + std::to_string(literalVector.size()) + ")");
  }

  Environment* env = new Environment(nullptr);

  std::vector<std::string>::iterator paramIter = parameterVector.begin();
  std::vector<Literal>::iterator literalIter = literalVector.begin();

  while(paramIter != parameterVector.end() && literalIter != literalVector.end()) {
  env->Define(Token(IDENTIFIER, *paramIter, *literalIter, -1), *literalIter);
    paramIter++;
    literalIter++;
  }

  //for recursion
  env->Define(Token(IDENTIFIER, "recurse", func, -1), func);

  //create the interpreter and call each line
  result = Literal();
  Interpreter interpreter(env);

  for (Stmt* stmt : reinterpret_cast<Block*>(func.GetBlock())->stmtVector) {
    interpreter.Execute(stmt);

    //check for a call to return
    if (interpreter.GetReturnCalled()) {
      result = interpreter.GetResult();
      break;
    }
  }

  //TODO: extract from env?
}

bool Interpreter::IsEqual(Literal lhs, Literal rhs) {
  //dereference references
  lhs = Dereference(lhs);
  rhs = Dereference(rhs);

  //check for undefined values
  if (lhs.GetType() == Literal::Type::UNDEFINED) {
    //'undefined' returns true only when compared to itself
    return rhs.GetType() == Literal::Type::UNDEFINED;
  }
  else if (rhs.GetType() == Literal::Type::UNDEFINED) {
    //only reaches here if lhs != undefined
    return false;
  }

  //boolean specific checks
  else if (lhs.GetType() == Literal::Type::BOOLEAN) {
    return lhs.GetBoolean() == IsTruthy(rhs);
  }
  else if (rhs.GetType() == Literal::Type::BOOLEAN) {
    return IsTruthy(lhs) == rhs.GetBoolean();
  }

  //type specific
  else if (lhs.GetType() == Literal::Type::NUMBER && rhs.GetType() == Literal::Type::NUMBER) {
    return lhs.GetNumber() == rhs.GetNumber();
  }
  else if (lhs.GetType() == Literal::Type::STRING && rhs.GetType() == Literal::Type::STRING) {
    return lhs.GetString() == rhs.GetString();
  }
  //TODO: objects?

  //catch all
  return false;
}

void Interpreter::CheckOperandsAreNumbers(Token op, Literal lhs, Literal rhs) {
  //dereference references
  lhs = Dereference(lhs);
  rhs = Dereference(rhs);

  if (lhs.GetType() == Literal::Type::NUMBER && rhs.GetType() == Literal::Type::NUMBER) {
    return;
  }

  throw RuntimeError(op.GetLine(), std::string() + "Operands of '" + op.GetLexeme() + "' must be numbers");
}

bool Interpreter::IsTruthy(Literal literal) {
  //dereference references
  literal = Dereference(literal);

  if (literal.GetType() == Literal::Type::UNDEFINED) {
    return false;
  }

  if (literal.GetType() == Literal::Type::BOOLEAN) {
    return literal.GetBoolean();
  }

  return true;
}

Literal Interpreter::Dereference(Literal literal) {
  //many levels of indirection
  while (literal.GetType() == Literal::Type::REFERENCE) {
    literal = *(literal.GetReference());
  }
  return literal;
}
