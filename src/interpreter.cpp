#include "interpreter.hpp"

#include "ast_deleter.hpp"
#include "ast_reader.hpp"
#include "error_handler.hpp"
#include "expr_visitors.hpp"
#include "runtime_error.hpp"

//TMP
#include <iostream>
#include <functional>

Interpreter::Interpreter(Environment* parent, Environment* forced) {
  environment = forced ? forced : new Environment(parent);
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
  ASTReaderPrefix reader; //debugging

  //for block contexts
  breakCalled = false;
  continueCalled = false;
  returnCalled = false;

  try {
std::cout << "READER:";
reader.Print(stmt);
std::cout << std::endl;

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

void Interpreter::Visit(Array* expr) {
  std::vector<Literal> literalVector;
  for (Expr* ptr : expr->exprVector) {
    Evaluate(ptr);
    literalVector.push_back(result);
  }
  result = Literal(literalVector);
}

void Interpreter::Visit(Assign* expr) {
  //get the assignment target type
  TokenTypeGetter typeGetter;
  expr->target->Accept(&typeGetter);

  if (typeGetter.GetType() != IDENTIFIER && typeGetter.GetType() != INDEX && typeGetter.GetType() != STAR) {
    throw RuntimeError(expr->op.GetLine(), "Unexpected type on left hand side of assignment");
  }

  //assign to a variable
  if (typeGetter.GetType() == IDENTIFIER) {
    Evaluate(expr->value);
    environment->Assign(dynamic_cast<Variable*>(expr->target)->name, result);
  }

  //assign to an index
  else if (typeGetter.GetType() == INDEX) {
    Index* index = dynamic_cast<Index*>(expr->target);

    //get info about the lhs
    TokenTypeGetter typeGetter;
    index->array->Accept(&typeGetter);

    //ugly lambda
    auto setter = [&](Literal array, Literal index, Literal value, int line) -> Literal {
      if (array.GetType() != Literal::Type::ARRAY) {
        throw RuntimeError(line, "Value left of '[' is not an array"); 
      }

      if (index.GetType() != Literal::Type::NUMBER) {
        throw RuntimeError(line, "Expected a number between '[' and ']'");
      }

      std::vector<Literal> literalVector = array.GetLiteralVector();

      //bounds check
      if (index.GetNumber() < 0 || index.GetNumber() >= literalVector.size()) {
        throw RuntimeError(line, "Array index out of bounds");
      }

      literalVector[(int)(index.GetNumber())] = value;
      array.SetLiteralVector(literalVector);

      return array;
    };

    //raw array
    if (typeGetter.GetType() == ARRAY) {
      //use "setter()" lambda
      Evaluate(index->array);
      Literal array = result;
      Evaluate(index->index);
      Literal index = result;
      Evaluate(expr->value);

      result = setter(array, index, result, expr->op.GetLine());
    }

    //extract and re-store the variable
    else if (typeGetter.GetType() == IDENTIFIER) {
      Literal array = environment->GetVar(dynamic_cast<Variable*>(index->array)->name);
      Evaluate(index->index);
      Literal indexNumber = result;
      Evaluate(expr->value);

      result = setter(array, indexNumber, result, expr->op.GetLine());

      environment->Assign(dynamic_cast<Variable*>(index->array)->name, result);
    }
  }

  //assign to a dereferenced variable
  else if (typeGetter.GetType() == STAR) {
    std::string starLexeme;

    //HACK: get the number of dereference stars
    int starCount = dynamic_cast<Unary*>(expr->target)->op.GetLiteral().GetNumber();
    Literal literal = environment->GetVar(dynamic_cast<Variable*>(dynamic_cast<Unary*>(expr->target)->rhs)->name); //base reference before dereferencing

    while(starCount > 0) {
      //display
      starLexeme += "*";

      //check that the literal type is a reference
      if (literal.GetType() != Literal::Type::REFERENCE) {
        throw RuntimeError(expr->op.GetLine(), std::string() + "Expression '" + starLexeme + dynamic_cast<Variable*>(dynamic_cast<Unary*>(expr->target)->rhs)->name.GetLexeme() + "' is not a variable");
      }

      //set the concrete reference via one level of dereference
      if (starCount == 1) {
        Evaluate(expr->value);
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

  //access operator
  if (expr->op.GetType() == DOT) {
    TokenTypeGetter typeGetter;
    expr->rhs->Accept(&typeGetter);

    if (typeGetter.GetType() != IDENTIFIER && typeGetter.GetType() != INDEX && typeGetter.GetType() != INVOCATION) {
      throw RuntimeError(expr->op.GetLine(), std::string() + "Can't access a member of '" + lhs.ToString() + "' (type " + std::to_string(typeGetter.GetType()) + ")");
    }

    Evaluate(expr->rhs);

    return;
  }

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

    //error
    default:
      throw RuntimeError(expr->op.GetLine(), std::string() + "Unexpected binary operator '" + expr->op.GetLexeme() + "'");
  }
}

void Interpreter::Visit(Class* expr) {
  //create the environment and interpreter
  result = Literal();
  Environment* env = new Environment(nullptr);
  Interpreter interpreter(nullptr, env);

  //call the class definition as a pseudo-function
  for (Stmt* stmt : expr->block->stmtVector) {
    interpreter.Execute(stmt);
  }

  //create the new class literal from the environment
  result = Literal(env);
}

void Interpreter::Visit(Function* expr) {
  //make the function into a liteal
  result = Literal(expr->parameterVector, expr->block);
}

void Interpreter::Visit(Grouping* expr) {
  Evaluate(expr->inner);
}

void Interpreter::Visit(Index* expr) {
  Evaluate(expr->array);

  if (result.GetType() != Literal::Type::ARRAY) {
    throw RuntimeError(-1, std::string() + "'" + result.ToString() + "' is not an array");
  }

  Literal array = result;

  Evaluate(expr->index);

  if (result.GetType() != Literal::Type::NUMBER) {
    throw RuntimeError(-1, std::string() + "Unexpected value type as index of array (expected number, got '" + result.ToString() + "'");
  }

  Literal index = result;

  result = array.GetLiteralVector()[(int)(index.GetNumber())];
}

void Interpreter::Visit(Invocation* expr) {
  //get the call requirements
  Evaluate(expr->expr);

  //invoke a function
  if (result.GetType() == Literal::Type::FUNCTION) {
    Literal func = result;

    std::vector<Literal> literalVector;
    for (Expr* ptr : expr->exprVector) {
      Evaluate(ptr);
      literalVector.push_back(result);
    }

    //finally
    CallFunction(func, literalVector);
  }

  //invoke a class
  else if (result.GetType() == Literal::Type::CLASS) {
    Literal prototype = result;

    std::vector<Literal> literalVector;
    for (Expr* ptr : expr->exprVector) {
      Evaluate(ptr);
      literalVector.push_back(result);
    }

    //finally
    CreateObject(prototype, literalVector);
  }

  //others
  else {
    throw RuntimeError(-1, std::string() + "'" + result.ToString() + "' is not a function or a class");
  }
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
void Interpreter::CallFunction(Literal func, std::vector<Literal> literalVector, Literal* self) {
  //build the environment
  std::vector<std::string> parameterVector = func.GetParameterVector();

  if (parameterVector.size() != literalVector.size()) {
    //TODO: much better error message
    throw RuntimeError(-1, std::string() + "Invalid number of arguments (expected " + std::to_string(parameterVector.size()) + ", received " + std::to_string(literalVector.size()) + ")");
  }

  //create the environment object
  Environment* env = new Environment(nullptr);

  std::vector<std::string>::iterator paramIter = parameterVector.begin();
  std::vector<Literal>::iterator literalIter = literalVector.begin();

  while(paramIter != parameterVector.end() && literalIter != literalVector.end()) {
  env->Define(Token(IDENTIFIER, *paramIter, *literalIter, -1), *literalIter);
    paramIter++;
    literalIter++;
  }

  //if calling on a method
  if (self != nullptr) {
    env->Define(Token(REFERENCE, "this", self, -1), self);
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
  if (self != nullptr) {
    result = *self;
  }
}

void Interpreter::CreateObject(Literal prototype, std::vector<Literal> literalVector) {
  //create the new object based on the prototype
  result = Literal();
  Literal object = prototype;
  object.SetType(Literal::Type::OBJECT);

  //call the constructor
  if (object.GetMember("create").GetType() == Literal::Type::FUNCTION) {
    CallFunction(object.GetMember("create"), literalVector, &object);
  }

  result = object;
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
