#include "interpreter.hpp"

#include "error_handler.hpp"

//TMP
#include <iostream>

void Interpreter::Execute(Stmt* stmt) {
  try {
    stmt->Accept(this);
    //TMP
    std::cout << "INTERPRETER:" << result.ToString() << std::endl;
  }
  catch(InterpreterError ie) {
    ErrorHandler::Error(ie.GetLine(), ie.GetErrMsg());
  }
}

void Interpreter::Evaluate(Expr* expr) {
  expr->Accept(this);
}

void Interpreter::Visit(Stmt* stmt) {
  //should never happen
  throw InterpreterError(-1, "Empty statement in AST"); 
}

void Interpreter::Visit(Expression* stmt) {
  Evaluate(stmt->expr);
}

void Interpreter::Visit(Expr* expr) {
  //should never happen
  throw InterpreterError(-1, "Empty expression in AST"); 
}

void Interpreter::Visit(Binary* expr) {
  Literal lhs;
  Literal rhs;

  //evaluate each side
  Evaluate(expr->lhs);
  lhs = result;
  Evaluate(expr->rhs);
  rhs = result;

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
        throw InterpreterError(expr->op.GetLine(), std::string() + "Operands of '" + expr->op.GetLexeme() + "' must be both numbers or both strings");
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
      throw InterpreterError(expr->op.GetLine(), std::string() + "Unexpected binary operator '" + expr->op.GetLexeme() + "'");
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
      throw InterpreterError(expr->op.GetLine(), std::string() + "Unexpected logical operator '" + expr->op.GetLexeme() + "'");
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
        throw InterpreterError(expr->op.GetLine(), "Can only negate a number");
      }
    break;

    case BANG:
      result = !IsTruthy(result);
    break;
  }
}

void Interpreter::Visit(Value* expr) {
  result = expr->value;
}

//helpers
bool Interpreter::IsEqual(Literal lhs, Literal rhs) {
  if (lhs.GetType() == Literal::Type::BOOLEAN && rhs.GetType() == Literal::Type::BOOLEAN) {
    return lhs.GetBoolean() == rhs.GetBoolean();
  }

  else if (lhs.GetType() == Literal::Type::NUMBER && rhs.GetType() == Literal::Type::NUMBER) {
    return lhs.GetNumber() == rhs.GetNumber();
  }

  else if (lhs.GetType() == Literal::Type::STRING && rhs.GetType() == Literal::Type::STRING) {
    return lhs.GetString() == rhs.GetString();
  }

  else if (lhs.GetType() == Literal::Type::UNDEFINED && rhs.GetType() == Literal::Type::UNDEFINED) {
    return true;
  }

  return false;
}

void Interpreter::CheckOperandsAreNumbers(Token op, Literal lhs, Literal rhs) {
  if (lhs.GetType() == Literal::Type::NUMBER && rhs.GetType() == Literal::Type::NUMBER) {
    return;
  }
  throw InterpreterError(op.GetLine(), std::string() + "Operands of '" + op.GetLexeme() + "' must be numbers");
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

