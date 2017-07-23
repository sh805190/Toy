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
  SetResult(nullptr);

  //delete the environment and all it's parents
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
ASTReaderPrefix reader;

std::cout << "READER:";
reader.Print(stmt);
std::cout << std::endl;

    SetResult(nullptr);
    stmt->Accept(this);

std::cout << "INTERPRETER: " << GetResult()->ToString() << std::endl;
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

  if (IsTruthy(GetResult())) {
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
  SetResult(nullptr); //explicitly undefined

  if (stmt->result) {
    Evaluate(stmt->result);
  }

  returnCalled = true;
}

void Interpreter::Visit(Var* stmt) {
  //define this variable
  if (stmt->initializer != nullptr) {
std::cout << "Defining a variable:";
    Evaluate(stmt->initializer);
std::cout << GetResult()->ToString() << std::endl;
    environment->Define(stmt->name, GetResult());
  } 
  else {
    //undefined literal
    environment->Define(stmt->name, nullptr);
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
    if (!IsTruthy(GetResult())) {
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
  SetResult(nullptr);
}

void Interpreter::Visit(Array* expr) {
  std::vector<Literal*> literalVector;
  for (Expr* ptr : expr->exprVector) {
    Evaluate(ptr);
    literalVector.push_back(GetResult()->Copy());
  }
  SetResult(new lArray(literalVector));
}

void Interpreter::Visit(Assign* expr) {
std::cout << "DEBUG (assign)";
  //get the assignment target type
  TokenTypeGetter typeGetter;
  expr->target->Accept(&typeGetter);

  if (typeGetter.GetType() != IDENTIFIER && typeGetter.GetType() != INDEX && typeGetter.GetType() != STAR && typeGetter.GetType() != DOT) {
    throw RuntimeError(expr->target->line, "Unexpected type on left hand side of assignment");
  }

  //assign to a variable
  if (typeGetter.GetType() == IDENTIFIER) {
    Evaluate(expr->value);
std::cout << "DEBUG (assign to an identifier):" << GetResult()->ToString() << std::endl;
    environment->Assign(static_cast<Variable*>(expr->target)->name, GetResult());
  }

  //assign to an index
  else if (typeGetter.GetType() == INDEX) {
std::cout << "DEBUG (assign to an index)";
    Index* index = static_cast<Index*>(expr->target);

    //get info about the lhs
    TokenTypeGetter typeGetter;
    index->array->Accept(&typeGetter);

    //ugly lambda
    auto setter = [&](lArray* array, lNumber* index, Literal* value, int line) -> Literal* {
      if (array->type != Literal::Type::LARRAY) {
        throw RuntimeError(line, "Value left of '[' is not an array"); 
      }

      if (index->type != Literal::Type::LNUMBER) {
        throw RuntimeError(line, "Expected a number between '[' and ']'");
      }

      //bounds check
      if (index->number < 0 || index->number >= array->array.size()) {
        throw RuntimeError(line, "Array index out of bounds");
      }

      delete array->array[(int)(index->number)];
      array->array[(int)(index->number)] = value;

      return array;
    };

    //raw array
    if (typeGetter.GetType() == ARRAY) {
      //use "setter()" lambda
      Evaluate(index->array);
      Literal* array = GetResult()->Copy();
      Evaluate(index->index);
      Literal* index = GetResult()->Copy();
      Evaluate(expr->value);

      SetResult(setter(static_cast<lArray*>(array), static_cast<lNumber*>(index), GetResult(), expr->line));
      delete array;
      delete index;
    }

    //extract and re-store the variable
    else if (typeGetter.GetType() == IDENTIFIER) {
      Literal* array = environment->GetVar(static_cast<Variable*>(index->array)->name);
      Evaluate(index->index);
      Literal* indexNumber = GetResult()->Copy();
      Evaluate(expr->value);

      SetResult(setter(static_cast<lArray*>(array), static_cast<lNumber*>(indexNumber), GetResult(), expr->line));

      environment->Assign(static_cast<Variable*>(index->array)->name, GetResult());

      delete indexNumber;
    }
  }

  //assign to a dereferenced variable
  else if (typeGetter.GetType() == STAR) {
std::cout << "DEBUG (assign to a reference)";
    std::string starLexeme;

    //HACK: get the number of 
    int starCount = static_cast<lNumber*>(static_cast<Unary*>(expr->target)->op.GetLiteral())->number;
    Literal* literal = environment->GetRef(static_cast<Variable*>(static_cast<Unary*>(expr->target)->rhs)->name); //base reference before dereferencing

    while(starCount > 0) {
      //display
      starLexeme += "*";

      //check that the literal type is a reference
      if (literal->type != Literal::Type::LREFERENCE) {
        throw RuntimeError(expr->line, std::string() + "Expression '" + starLexeme + static_cast<Variable*>(static_cast<Unary*>(expr->target)->rhs)->name.GetLexeme() + "' is not a variable");
      }

      //set the concrete reference via one level of dereference
      if (starCount == 1) {
        Evaluate(expr->value);
        delete static_cast<lReference*>(literal)->reference;
        static_cast<lReference*>(literal)->reference = GetResult();
        break;
      }

      //one level of dereference
      literal = static_cast<lReference*>(literal)->reference;

      starCount--;
    }
  }

  else if (typeGetter.GetType() == DOT) {
std::cout << "DEBUG: ASSIGNING TO A MEMBER" << std::endl;
    throw RuntimeError(expr->line, "Assigning to a member not implemented yet");
  }

  //something isn't working
  else {
    throw RuntimeError(expr->line, "Assignment failed for an unknown reason");
  }
}

void Interpreter::Visit(Binary* expr) {
  //evaluate lhs
  Evaluate(expr->lhs);
  Literal* lhs = GetResult()->Copy();

  //access member
  if (expr->op.GetType() == DOT) {
    //check the rhs type
    TokenTypeGetter typeGetter;
    expr->rhs->Accept(&typeGetter);
    if (typeGetter.GetType() == IDENTIFIER) {
      Token name = static_cast<Variable*>(expr->rhs)->name;
      SetResult(static_cast<lObject*>(lhs)->members[name.GetLexeme()]);
      delete lhs;
      return;
    }

    //error
    throw RuntimeError(expr->line, std::string() + "Can't access member of an object (type " + std::to_string(typeGetter.GetType()) + ")");
  }

  //get the rhs for all other operations
  Evaluate(expr->rhs);
  Literal* rhs = GetResult();

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
    case MINUS:
      CheckOperandsAreNumbers(expr->op, lhs, rhs);
      SetResult(new lNumber(static_cast<lNumber*>(lhs)->number - static_cast<lNumber*>(rhs)->number));
    break;

    case PLUS: //special case for the plus operator
      lhs = Dereference(lhs);
      rhs = Dereference(rhs);

      if (lhs->type == Literal::Type::LNUMBER && rhs->type == Literal::Type::LNUMBER) {
        SetResult(new lNumber(static_cast<lNumber*>(lhs)->number + static_cast<lNumber*>(rhs)->number));
      }
      else if (lhs->type == Literal::Type::LSTRING && rhs->type == Literal::Type::LSTRING) {
        SetResult(new lString(static_cast<lString*>(lhs)->str + static_cast<lString*>(rhs)->str));
      }
      else {
        throw RuntimeError(expr->line, std::string() + "Operands of '" + expr->op.GetLexeme() + "' must be both numbers or both strings");
      }
    break;

    case SLASH:
      CheckOperandsAreNumbers(expr->op, lhs, rhs);
      SetResult(new lNumber(static_cast<lNumber*>(lhs)->number / static_cast<lNumber*>(rhs)->number));
    break;

    case STAR:
      CheckOperandsAreNumbers(expr->op, lhs, rhs);
      SetResult(new lNumber(static_cast<lNumber*>(lhs)->number * static_cast<lNumber*>(rhs)->number));
    break;

    //error
    default:
      throw RuntimeError(expr->op.GetLine(), std::string() + "Unexpected binary operator '" + expr->op.GetLexeme() + "'");
  }

  delete lhs;
  delete rhs;
}

void Interpreter::Visit(Class* expr) {
  //create the environment and interpreter
  SetResult(nullptr);
  Environment* env = new Environment(nullptr);
  Interpreter interpreter(nullptr, env);

  //call the class definition as a pseudo-function
  for (Stmt* stmt : expr->block->stmtVector) {
    interpreter.Execute(stmt);
  }

  //create the new class literal from the environment
  SetResult(new lClass( (*env->GetLiteralMap()) ));

  delete env;
}

void Interpreter::Visit(Function* expr) {
  //make the function into a literal
  SetResult(new lFunction(expr->parameterVector, expr->block));
}

void Interpreter::Visit(Grouping* expr) {
  Evaluate(expr->inner);
}

void Interpreter::Visit(Index* expr) {
  Evaluate(expr->array);

  if (GetResult()->type != Literal::Type::LARRAY) {
    throw RuntimeError(expr->line, std::string() + "'" + GetResult()->ToString() + "' is not an array");
  }

  Literal* array = GetResult()->Copy();

  Evaluate(expr->index);

  if (GetResult()->type != Literal::Type::LNUMBER) {
    throw RuntimeError(expr->line, std::string() + "Unexpected value type as index of array (expected number, got '" + GetResult()->ToString() + "'");
  }

  Literal* index = GetResult()->Copy();

  SetResult( static_cast<lArray*>(array)->array[(int)( static_cast<lNumber*>(index)->number)]->Copy());

  delete array;
  delete index;
}

void Interpreter::Visit(Invocation* expr) {
  //get the call requirements
  Evaluate(expr->expr);

  //invoke a function
  if (GetResult()->type == Literal::Type::LFUNCTION) {
    Literal* func = GetResult()->Copy();

    std::vector<Literal*> literalVector;
    for (Expr* ptr : expr->exprVector) {
      Evaluate(ptr);
      literalVector.push_back(GetResult()->Copy());
    }

    //finally
    CallFunction(expr->line, static_cast<lFunction*>(func), literalVector);

    delete func;
    for (Literal* ptr : literalVector) {
      delete ptr;
    }
  }

  //invoke a class
  else if (GetResult()->type == Literal::Type::LCLASS) {
    Literal* prototype = GetResult()->Copy();

    std::vector<Literal*> literalVector;
    for (Expr* ptr : expr->exprVector) {
      Evaluate(ptr);
      literalVector.push_back(GetResult()->Copy());
    }

    //finally
    CreateObject(expr->line, static_cast<lClass*>(prototype), literalVector);

    delete prototype;
    for (Literal* ptr : literalVector) {
      delete ptr;
    }
  }

  //others
  else {
    throw RuntimeError(expr->line, std::string() + "'" + GetResult()->ToString() + "' is not a function or a class");
  }
}

void Interpreter::Visit(Logical* expr) {
  //evaluate the first side
  Evaluate(expr->lhs);

  switch(expr->op.GetType()) {
    //equality operators
    case AND:
      if (IsTruthy(GetResult())) {
        Evaluate(expr->rhs);
      }
    break;

    case OR:
      if (IsTruthy(GetResult())) {
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
      if (GetResult()->type == Literal::Type::LNUMBER) {
        SetResult(new lNumber(-(static_cast<lNumber*>(GetResult())->number)));
      }
      else {
        throw RuntimeError(expr->op.GetLine(), "Can only negate a number");
      }
    break;

    case BANG:
      SetResult(new lBoolean(!IsTruthy(GetResult())));
    break;

    case AMPERSAND:
      SetResult(environment->GetRef(static_cast<Variable*>(expr->rhs)->name));
    break;

    case STAR: {
      std::string starLexeme;
      int starCount = static_cast<lNumber*>(expr->op.GetLiteral())->number;
      Literal* literal = GetResult();
      while(starCount > 0) {
        //display
        starLexeme += "*";

        //check that the literal type is a reference
        if (literal->type != Literal::Type::LREFERENCE) {
          Variable* variable = static_cast<Variable*>(expr->rhs);
          throw RuntimeError(variable->name.GetLine(), std::string() + "Expression '" + starLexeme + variable->name.GetLexeme() + "' is not a variable");
        }

        //one level of dereference
        literal = static_cast<lReference*>(literal)->reference;

        starCount--;
      }
      SetResult(literal);
    }
    break;
  }
}

void Interpreter::Visit(Value* expr) {
  SetResult(expr->value);
}

void Interpreter::Visit(Variable* expr) {
  SetResult(environment->GetVar(expr->name));
}

//helpers
void Interpreter::CallFunction(int line, lFunction* func, std::vector<Literal*> literalVector, lObject* self) {
  if (func->parameters.size() != literalVector.size()) {
    //TODO: much better error message
    throw RuntimeError(line, std::string() + "Invalid number of arguments (expected " + std::to_string(func->parameters.size()) + ", received " + std::to_string(literalVector.size()) + ")");
  }

  //create the environment object
  Environment* env = new Environment(nullptr);

  std::vector<std::string>::iterator paramIter = func->parameters.begin();
  std::vector<Literal*>::iterator literalIter = literalVector.begin();

  while(paramIter != func->parameters.end() && literalIter != literalVector.end()) {
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
  SetResult(nullptr);
  Interpreter interpreter(env);

  for (Stmt* stmt : static_cast<Block*>(func->block)->stmtVector) {
    interpreter.Execute(stmt);

    //check for a call to return
    if (interpreter.GetReturnCalled()) {
      SetResult(interpreter.GetResult()->Copy());
      break;
    }
  }

  //TODO: extract from env?
  if (self != nullptr) {
    SetResult(self);
  }
}

void Interpreter::CreateObject(int line, lClass* prototype, std::vector<Literal*> literalVector) {
  //create the new object based on the prototype
  SetResult(nullptr);
  lObject* object = new lObject(prototype->members);

  //call the constructor
  if (object->members["create"]->type == Literal::Type::LFUNCTION) {
    CallFunction(line, static_cast<lFunction*>(object->members["create"]), literalVector, object);
  }

  SetResult(static_cast<Literal*>(object));
}

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
