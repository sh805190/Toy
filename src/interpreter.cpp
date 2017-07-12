#include "interpreter.hpp"

#include "error_handler.hpp"
#include "runtime_error.hpp"

//TMP
#include <iostream>

void Interpreter::Execute(Stmt* stmt) {
  try {
    result = Literal();
    stmt->Accept(this);
    //TMP
    std::cout << "INTERPRETER:" << result.ToString() << std::endl;
  }
  catch(RuntimeError ie) {
    ErrorHandler::Error(ie.GetLine(), ie.GetErrMsg());
  }
}

void Interpreter::Evaluate(Expr* expr) {
  expr->Accept(this);
}

void Interpreter::Visit(Stmt* stmt) {
  //should never happen
  throw RuntimeError(-1, "Empty statement in AST"); 
}

void Interpreter::Visit(Expression* stmt) {
  Evaluate(stmt->expr);
}

void Interpreter::Visit(Var* stmt) {
  //define this variable
  if (stmt->initializer != nullptr) {
    Evaluate(stmt->initializer);
    environment.Define(stmt->name, result);
  } 
  else {
    //undefined literal
    environment.Define(stmt->name, Literal());
  }
}

void Interpreter::Visit(Expr* expr) {
  //empty = undefined
  result = Literal();
}

void Interpreter::Visit(Assign* expr) {
  Evaluate(expr->value);
  environment.Assign(expr->name, result);
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

void Interpreter::Visit(Grouping* expr) {
  Evaluate(expr->inner);
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
        result = IsTruthy(result);
      }
      else {
        result = false;
      }
    break;

    case OR:
      if (IsTruthy(result)) {
        result = true;
      }
      else {
        Evaluate(expr->rhs);
        result = IsTruthy(result);
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
      result = environment.GetRef(dynamic_cast<Variable*>(expr->rhs)->name);
    break;

    case STAR:
      //TODO
    break;
  }
}

void Interpreter::Visit(Value* expr) {
  result = expr->value;
}

void Interpreter::Visit(Variable* expr) {
  result = environment.GetVar(expr->name);
}

//helpers
bool Interpreter::IsEqual(Literal lhs, Literal rhs) {
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
  //TODO: references, objects?

  //catch all
  return false;
}

void Interpreter::CheckOperandsAreNumbers(Token op, Literal lhs, Literal rhs) {
  if (lhs.GetType() == Literal::Type::NUMBER && rhs.GetType() == Literal::Type::NUMBER) {
    return;
  }
  throw RuntimeError(op.GetLine(), std::string() + "Operands of '" + op.GetLexeme() + "' must be numbers");
}

bool Interpreter::IsTruthy(Literal literal) {
  if (literal.GetType() == Literal::Type::BOOLEAN) {
    return literal.GetBoolean();
  }

  if (literal.GetType() == Literal::Type::UNDEFINED) {
    return false;
  }

  return true;
}

