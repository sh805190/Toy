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
  //TODO
  throw RuntimeError(stmt->line, "Block is not yet implemented");
}

void Interpreter::Visit(Break* stmt) {
  //TODO
  throw RuntimeError(stmt->line, "Break is not yet implemented");
}

void Interpreter::Visit(Continue* stmt) {
  //TODO
  throw RuntimeError(stmt->line, "Continue is not yet implemented");
}

void Interpreter::Visit(Expression* stmt) {
  Evaluate(stmt->expr);
}

void Interpreter::Visit(If* stmt) {
  //TODO
  throw RuntimeError(stmt->line, "If is not yet implemented");
}

void Interpreter::Visit(Return* stmt) {
  //TODO
  throw RuntimeError(stmt->line, "Return is not yet implemented");
}

void Interpreter::Visit(Var* stmt) {
  //TODO
  throw RuntimeError(stmt->line, "Var is not yet implemented");
}

void Interpreter::Visit(While* stmt) {
  //TODO
  throw RuntimeError(stmt->line, "While is not yet implemented");
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
  //TODO
  throw RuntimeError(expr->line, "Assign is not yet implemented");
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
      CheckOperandsAreNumbers(expr->op, lhs, rhs);
      SetResult(new lNumber(static_cast<lNumber*>(lhs)->number + static_cast<lNumber*>(rhs)->number));
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
  //TODO
  throw RuntimeError(expr->line, "Logical is not yet implemented");
}

void Interpreter::Visit(Unary* expr) {
  //TODO
  throw RuntimeError(expr->line, "Unary is not yet implemented");
}

void Interpreter::Visit(Value* expr) {
  SetResult(expr->value);
}

void Interpreter::Visit(Variable* expr) {
  //TODO
  throw RuntimeError(expr->line, "Variable is not yet implemented");
}

//helpers
bool Interpreter::IsEqual(Literal* lhs, Literal* rhs) {
  //dereference references
  lhs = Dereference(lhs);
  rhs = Dereference(rhs);

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
  //dereference references
  lhs = Dereference(lhs);
  rhs = Dereference(rhs);

  if (lhs->type == Literal::Type::LNUMBER && rhs->type == Literal::Type::LNUMBER) {
    return;
  }

  throw RuntimeError(op.GetLine(), std::string() + "Operands of '" + op.GetLexeme() + "' must be numbers");
}

bool Interpreter::IsTruthy(Literal* literal) {
  //dereference references
  literal = Dereference(literal);

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
