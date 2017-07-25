#include "interpreter.hpp"

#include "error_handler.hpp"
#include "expr_visitors.hpp"
#include "runtime_error.hpp"

#include <functional>

Interpreter::Interpreter(Environment* parent, Environment* forced) {
  environment = forced ? forced : new Environment(parent);
}

Interpreter::~Interpreter() {
  SetResult(nullptr);

  //delete the environment and all its parents
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
    SetResult(nullptr);
    stmt->Accept(this);
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

Literal* Interpreter::SetResult(Literal* ptr) {
  delete result;
  return result = ptr;
}

Literal* Interpreter::GetResult() {
  if (result == nullptr) {
    result = new lUndefined();
  }
  return result;
}

void Interpreter::Visit(Stmt* stmt) {
  //should never happen
  throw RuntimeError(stmt->line, "Empty statement in AST"); 
}

void Interpreter::Visit(Block* stmt) {
  environment = new Environment(environment);

  try {
    for (Stmt* stmtPtr : stmt->stmtVector) {
      Execute(stmtPtr);

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

  //decrease the scope
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
  Evaluate(stmt->condition);
  if (IsTruthy(GetResult())) {
    Execute(stmt->thenBranch);
  }
  else {
    if (stmt->elseBranch) {
      Execute(stmt->elseBranch);
    }
  }
}

void Interpreter::Visit(Return* stmt) {
  SetResult(nullptr); //explicitly undefined

  if (stmt->result) {
    Evaluate(stmt->result);
  }

  returnCalled = true;
}

void Interpreter::Visit(Var* stmt) {
  //declaring a variable
  if (stmt->initializer) {
    Evaluate(stmt->initializer);
    environment->Define(stmt->name, GetResult());
  }
  else {
    environment->Define(stmt->name, nullptr);
  }
}

void Interpreter::Visit(While* stmt) {
  loopDepth++; //diagnostics

  for (;;) {
    //block contexts
    if (breakCalled) {
      breakCalled = false; //one level
      loopDepth--;
      break;
    }

    if (returnCalled) {
      loopDepth--;
      return; //don't reset the context flags
     }

    continueCalled = false;

    //if we should run another loop
    Evaluate(stmt->condition);

    if (!IsTruthy(GetResult())) {
      loopDepth--;
      break;
    }

    //execute the loop once
    Execute(stmt->branch);
  }

  //reset the flags after a block
  breakCalled = false;
  continueCalled = false;
  returnCalled = false;
}

void Interpreter::Visit(Expr* expr) {
  //TODO
  throw RuntimeError(expr->line, "Expr is not yet implemented");
}

void Interpreter::Visit(Array* expr) {
  //TODO
  throw RuntimeError(expr->line, "Array is not yet implemented");
}

void Interpreter::Visit(Assign* expr) {
  //get the assignment target type
  TokenTypeGetter typeGetter;
  expr->target->Accept(&typeGetter);

  //use a switch statement this time!
  switch(typeGetter.GetType()) {
    case IDENTIFIER: {
      Token name = static_cast<Variable*>(expr->target)->name;
      Evaluate(expr->value);
      environment->Assign(name, GetResult());
    }
    break;

    default:
      throw RuntimeError(expr->target->line, std::string() + "Unknown or unimplemented type on left hand side of assignment (type ID " + std::to_string(typeGetter.GetType()) + ")");
  }
}

void Interpreter::Visit(Binary* expr) {
  //evaluate each side
  Evaluate(expr->lhs);
  Literal* lhs = GetResult()->Copy();
  Evaluate(expr->rhs);
  Literal* rhs = GetResult()->Copy();

  //take action based on the operator
  switch(expr->op.GetType()) {
    //equality operators
    case EQUAL_EQUAL:
      SetResult(new lBoolean(IsEqual(lhs, rhs)));
    break;

    case BANG_EQUAL:
      SetResult(new lBoolean(!IsEqual(lhs, rhs)));
    break;

    //comparison operators
    case LESS:
      CheckOperandsAreNumbers(expr->op, lhs, rhs);
      SetResult(new lBoolean(static_cast<lNumber*>(lhs)->number < static_cast<lNumber*>(rhs)->number));
    break;

    case LESS_EQUAL:
      CheckOperandsAreNumbers(expr->op, lhs, rhs);
      SetResult(new lBoolean(static_cast<lNumber*>(lhs)->number <= static_cast<lNumber*>(rhs)->number));
    break;

    case GREATER:
      CheckOperandsAreNumbers(expr->op, lhs, rhs);
      SetResult(new lBoolean(static_cast<lNumber*>(lhs)->number > static_cast<lNumber*>(rhs)->number));
    break;

    case GREATER_EQUAL:
      CheckOperandsAreNumbers(expr->op, lhs, rhs);
      SetResult(new lBoolean(static_cast<lNumber*>(lhs)->number >= static_cast<lNumber*>(rhs)->number));
    break;

    //arithmatic operators
    case PLUS:
      //special checks for the PLUS operators, to support string concatenation
      if (lhs->type == Literal::Type::LNUMBER && rhs->type == Literal::Type::LNUMBER) {
        SetResult(new lNumber(static_cast<lNumber*>(lhs)->number + static_cast<lNumber*>(rhs)->number));
      }
      else if (lhs->type == Literal::Type::LSTRING && rhs->type == Literal::Type::LSTRING) {
        //concatenate strings
        SetResult(new lString(static_cast<lString*>(lhs)->str + static_cast<lString*>(rhs)->str));
      }
      else {
        throw RuntimeError(expr->line, std::string() + "Operands of '" + expr->op.GetLexeme() + "' must be both numbers or both strings");
      }
    break;

    case MINUS:
      CheckOperandsAreNumbers(expr->op, lhs, rhs);
      SetResult(new lNumber(static_cast<lNumber*>(lhs)->number - static_cast<lNumber*>(rhs)->number));
    break;

    case STAR:
      CheckOperandsAreNumbers(expr->op, lhs, rhs);
      SetResult(new lNumber(static_cast<lNumber*>(lhs)->number * static_cast<lNumber*>(rhs)->number));
    break;

    case SLASH:
      CheckOperandsAreNumbers(expr->op, lhs, rhs);
      SetResult(new lNumber(static_cast<lNumber*>(lhs)->number / static_cast<lNumber*>(rhs)->number));
    break;

    default:
      throw RuntimeError(expr->op.GetLine(), std::string() + "Unexpected binary operator '" + expr->op.GetLexeme() + "'");
  }

  //cleanup
  delete lhs;
  delete rhs;
}

void Interpreter::Visit(Class* expr) {
  //TODO
  throw RuntimeError(expr->line, "Class is not yet implemented");
}

void Interpreter::Visit(Function* expr) {
  //TODO
  throw RuntimeError(expr->line, "Function is not yet implemented");
}

void Interpreter::Visit(Grouping* expr) {
  Evaluate(expr->inner);
}

void Interpreter::Visit(Index* expr) {
  //TODO
  throw RuntimeError(expr->line, "Index is not yet implemented");
}

void Interpreter::Visit(Invocation* expr) {
  //TODO
  throw RuntimeError(expr->line, "Invocation is not yet implemented");
}

void Interpreter::Visit(Logical* expr) {
  //evaluate the left side
  Evaluate(expr->lhs);

  switch(expr->op.GetType()) {
    case AND:
      if (IsTruthy(GetResult())) {
        Evaluate(expr->rhs);
      }
    break;

    case OR:
      if (!IsTruthy(GetResult())) {
        Evaluate(expr->rhs);
      }
    break;

    default:
      throw RuntimeError(expr->op.GetLine(), std::string() + "Unexpected logical operator '" + expr->op.GetLexeme() + "' found");
  }
}

void Interpreter::Visit(Unary* expr) {
  Evaluate(expr->rhs);

  switch(expr->op.GetType()) {
    case MINUS:
      if (GetResult()->type != Literal::Type::LNUMBER) {
        throw RuntimeError(expr->op.GetLine(), "Can only negate a number");
      }
      SetResult(new lNumber( static_cast<lNumber*>(GetResult())->number ));
    break;

    case BANG:
      SetResult(new lBoolean( !IsTruthy(GetResult()) ));
    break;

    default:
      throw RuntimeError(expr->line, std::string() + "Unexpected unary operator '" + expr->op.GetLexeme() + "' found");
  }
}

void Interpreter::Visit(Value* expr) {
  //the literal value
  SetResult(expr->value->Copy());
}

void Interpreter::Visit(Variable* expr) {
  //retreive the value based on the variable name
  SetResult(environment->GetVar(expr->name));
}

//helpers
bool Interpreter::IsEqual(Literal* lhs, Literal* rhs) {
  //check for undefined values
  if (lhs->type == Literal::Type::LUNDEFINED) {
    //'undefined' returns true only when compared to itself
    return rhs->type == Literal::Type::LUNDEFINED;
  }
  else if (rhs->type == Literal::Type::LUNDEFINED) {
    //only reaches here if lhs != undefined
    return false;
  }

  //boolean specific checks
  else if (lhs->type == Literal::Type::LBOOLEAN) {
    return static_cast<lBoolean*>(lhs)->boolean == IsTruthy(rhs);
  }
  else if (rhs->type == Literal::Type::LBOOLEAN) {
    return IsTruthy(lhs) == static_cast<lBoolean*>(rhs)->boolean;
  }

  //type specific
  else if (lhs->type == Literal::Type::LNUMBER && rhs->type == Literal::Type::LNUMBER) {
    return static_cast<lNumber*>(lhs)->number == static_cast<lNumber*>(rhs)->number;
  }
  else if (lhs->type == Literal::Type::LSTRING && rhs->type == Literal::Type::LSTRING) {
    return static_cast<lString*>(lhs)->str == static_cast<lString*>(rhs)->str;
  }
  //TODO: objects?

  //catch all
  return false;
}

void Interpreter::CheckOperandsAreNumbers(Token op, Literal* lhs, Literal* rhs) {
  if (lhs->type == Literal::Type::LNUMBER && rhs->type == Literal::Type::LNUMBER) {
    return;
  }

  throw RuntimeError(op.GetLine(), std::string() + "Operands of '" + op.GetLexeme() + "' must be numbers");
}

bool Interpreter::IsTruthy(Literal* literal) {
  if (literal->type == Literal::Type::LUNDEFINED) {
    return false;
  }

  if (literal->type == Literal::Type::LBOOLEAN) {
    return static_cast<lBoolean*>(literal)->boolean;
  }

  return true;
}

Literal* Interpreter::Dereference(Literal* literal) {
  //many levels of indirection
  if (literal->type == Literal::Type::LREFERENCE) {
    return Dereference(static_cast<lReference*>(literal)->reference);
  }
  return literal;
}
