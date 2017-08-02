#include "interpreter.hpp"

#include "ast_duplicator.hpp"
#include "error_handler.hpp"
#include "expr_visitors.hpp"
#include "run.hpp"
#include "runtime_error.hpp"

#include <fstream>
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

bool Interpreter::SetFlag(std::string s, bool b) {
  return useFlag[s] = b;
}

bool Interpreter::GetFlag(std::string s) {
  if (useFlag.find(s) == useFlag.end()) {
    return useFlag[s] = false;
  }
  return useFlag[s];
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

void Interpreter::Visit(Use* stmt) {
  //catch all for generic commands
  if (stmt->command.GetType() == IDENTIFIER) { //TODO: specialized types
    if (stmt->command.GetLexeme() == "version") {
      CheckVersion(stmt->line, stmt->command.GetLiteral());
    }

    SetFlag(stmt->command.GetLiteral()->ToString(), true);
    return;
  }

  //handle modules
  else if (stmt->command.GetType() == STRING) {
    LoadModule(stmt->command.GetLiteral()->ToString());
    return;
  }

  throw RuntimeError(stmt->line, "Unknown command passed to use statement");
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

    case INDEX: {
      //ugly lambda
      auto setter = [&](lArray* array, lNumber* index, Literal* value, int line) -> Literal* {
        //error checking
        if (array->type != Literal::Type::LARRAY) {
          throw RuntimeError(line, "Expected an array to the left of '[' operator"); 
        }

        if (index->type != Literal::Type::LNUMBER) {
          throw RuntimeError(line, "Unexpected type between '[' and ']' operators (expected number)"); 
        }

        //bounds check
        if (index->number < 0 || index->number >= array->array.size()) {
          throw RuntimeError(line, "Array index out of bounds");
        }

        //finally, set the element's value
        delete array->array[(int)(index->number)];
        array->array[(int)(index->number)] = value->Copy();

        //return
        return array;
      };

      Index* index = static_cast<Index*>(expr->target);

      //get the info about the lhs
      TokenTypeGetter typeGetter;
      index->array->Accept(&typeGetter);

      switch(typeGetter.GetType()) {
        case ARRAY: {
          Evaluate(index->array);
          Literal* array = GetResult()->Copy();
          Evaluate(index->index);
          Literal* index = GetResult()->Copy();
          Evaluate(expr->value);
          SetResult(setter(static_cast<lArray*>(array), static_cast<lNumber*>(index), GetResult(), expr->line)->Copy());
          delete array;
          delete index;
        }
        break;

        case IDENTIFIER: {
          Literal* array = environment->GetRef(static_cast<Variable*>(index->array)->name);
          Evaluate(index->index);
          Literal* index = GetResult()->Copy();
          Evaluate(expr->value);
          SetResult(setter(static_cast<lArray*>(array), static_cast<lNumber*>(index), GetResult(), expr->line)->Copy());
          delete index;
        }
        break;

        default:
          throw RuntimeError(expr->line, std::string() + "Unexpected type found on lhs of index (type " + std::to_string((int)(typeGetter.GetType())) + ")");
      }
    }
    break;

    case STAR: {
      //dereference on lhs of assignment
      Unary* target = static_cast<Unary*>(expr->target);

      //get the star count and base literal
      int starCount = static_cast<lNumber*>( target->op.GetLiteral() )->number;
      Literal* literal = environment->GetRef(static_cast<Variable*>(target->rhs)->name);

      //dig down
      std::string starLexeme;
      while(starCount > 0) {
        //display
        starLexeme += "*";

        //check that the literal is a reference
        if (literal->type != Literal::Type::LREFERENCE) {
          std::string varName = static_cast<Variable*>(target->rhs)->name.GetLexeme();
          throw RuntimeError(expr->line, std::string() + "Expression '" + starLexeme + varName + "' is not a variable");
        }

        //set the literal reference via one level of indirection
        if (starCount == 1) {
          Evaluate(expr->value);
          delete static_cast<lReference*>(literal)->reference;
          static_cast<lReference*>(literal)->reference = GetResult()->Copy();
          break;
        }

        //one level of dereference
        literal = static_cast<lReference*>(literal)->reference;
        starCount--;
      }
    }
    break;

    default:
      throw RuntimeError(expr->target->line, std::string() + "Unknown or unimplemented type on left hand side of assignment (type ID " + std::to_string(typeGetter.GetType()) + ")");
  }
}

void Interpreter::Visit(Binary* expr) {
  //evaluate each side
  Evaluate(expr->lhs);
  Literal* lhsResult = GetResult()->Copy();
  Evaluate(expr->rhs);
  Literal* rhsResult = GetResult()->Copy();

  //dereference
  Literal* lhs = Dereference(lhsResult);
  Literal* rhs = Dereference(rhsResult);

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
      //emergency cleanup
      delete lhsResult;
      delete rhsResult;
      throw RuntimeError(expr->op.GetLine(), std::string() + "Unexpected binary operator '" + expr->op.GetLexeme() + "'");
  }

  //cleanup
  delete lhsResult;
  delete rhsResult;
}

void Interpreter::Visit(Class* expr) {
  //TODO
  throw RuntimeError(expr->line, "Class is not yet implemented");
}

void Interpreter::Visit(Function* expr) {
  //make the function into a literal
  ASTDuplicator dup;
  SetResult(new lFunction(expr->parameterVector, dup.DuplicateAST(static_cast<Block*>(expr->block)) ));
}

void Interpreter::Visit(Grouping* expr) {
  Evaluate(expr->inner);
}

void Interpreter::Visit(Index* expr) {
  //get the array
  Evaluate(expr->array);

  //error checking
  if (GetResult()->type != Literal::Type::LARRAY) {
    throw RuntimeError(expr->line, std::string() + "'" + GetResult()->ToString() + "' is not an array");
  }

  Literal* array = GetResult()->Copy();

  //get the index
  Evaluate(expr->index);

  //error checking
  if (GetResult()->type != Literal::Type::LNUMBER) {
    throw RuntimeError(expr->line, std::string() + "Unexpected value type as index of array (expected number, got '" + GetResult()->ToString() + "')");
  }

  Literal* index = GetResult()->Copy();

  //set result
  SetResult(static_cast<lArray*>(array)->array[(int)(static_cast<lNumber*>(index)->number)]->Copy());

  //cleanup
  delete array;
  delete index;
}

void Interpreter::Visit(Invocation* expr) {
  //evaluate the call requirements
  Evaluate(expr->expr);

  //fallthrough logic
  bool deleteCopy = false;
  Literal* func = nullptr;

  //determine what to do
  switch(GetResult()->type) {
    //invoke a reference
    case Literal::Type::LREFERENCE: {
      func = Dereference(GetResult());
    }
    //allow fallthrough

    //invoke a function
    case Literal::Type::LFUNCTION: {
      if (!func) {
        deleteCopy = true;
        func = GetResult()->Copy();
      }

      //evaluate and record each parameter
      std::vector<Literal*> literalVector;
      for (Expr* ptr : expr->exprVector) {
        Evaluate(ptr);
        literalVector.push_back(GetResult()->Copy());
      }

      //execute
      CallFunction(expr->line, static_cast<lFunction*>(func), literalVector);

      //cleanup
      for (Literal* ptr : literalVector) {
        delete ptr;
      }
      if (deleteCopy) {
        delete func;
      }
    }
    break;

    default:
      throw RuntimeError(expr->line, std::string() + "'" + GetResult()->ToString() + "' is not a function");
  }
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

    case AMPERSAND:
      SetResult( new lReference(environment->GetRef( static_cast<Variable*>(expr->rhs)->name )) );
    break;

    case STAR: {
      //dereference in an expression (rhs of assignment, etc.)
      std::string starLexeme;
      int starCount = static_cast<lNumber*>(expr->op.GetLiteral())->number;
      Literal* literal = GetResult();
      while(starCount > 0) {
        //display
        starLexeme += "*";

        //check that the literal type is a reference
        if (literal->type != Literal::Type::LREFERENCE) {
          Variable* variable = static_cast<Variable*>(expr->rhs);
          throw RuntimeError(expr->line, std::string() + "Expression '" + starLexeme + variable->name.GetLexeme() + "' is not a variable");
        }

        //one level of dereference
        literal = static_cast<lReference*>(literal)->reference;
        starCount--;
      }

      SetResult(literal->Copy());
    }
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

void Interpreter::CheckVersion(int line, Literal* literal) {
  switch(literal->type) {
    case Literal::Type::LNUMBER: {
      double d = static_cast<lNumber*>(literal)->number;

      if (d == 0.0) return;

      throw RuntimeError(line, std::string() + "Incorrect toy version detected (can't run version " + literal->ToString() + ")");
    }
    break;

    //specialized versions
    case Literal::Type::LSTRING:
      throw RuntimeError(line, std::string() + "Unrecognized version '" + literal->ToString() + "'");
    break;
  }
}

void Interpreter::LoadModule(std::string moduleName) {
  if (LoadBuiltinModule(moduleName)) return;

  //error handling
  int errorCount = ErrorHandler::GetErrorCount();

  //load the file
  std::string source;
  std::ifstream is(moduleName, std::fstream::in);

  if (!is.is_open()) {
    throw RuntimeError(-1, std::string() + "Could not open module '" + moduleName + "'");
  }

  getline(is, source, '\0');
  is.close();

  //mock the environment
  Environment* env = new Environment(nullptr);

  //run the file in a sub-interpreter
  Interpreter* interpreter = new Interpreter(nullptr, env);
  run(source, interpreter, true);

  //check for panic state
  if (ErrorHandler::GetErrorCount() - errorCount && interpreter->GetFlag("panic")) {
    delete interpreter;
    throw RuntimeError(-1, std::string() + "Panic state detected in module '" + moduleName + "'");
  }

  //handle returns
  if (interpreter->GetReturnCalled()) {
    SetResult(interpreter->GetResult()->Copy());
  }

  //if error state
  if (ErrorHandler::GetErrorCount() - errorCount) {
    throw RuntimeError(-1, std::string() + "Error state detected in module '" + moduleName + "'");
  }

  //absorb the result
  environment->Absorb(env);

  //the interpeter will delete the environment itself
  delete interpreter;
}

bool Interpreter::LoadBuiltinModule(std::string moduleName) {
  //TODO
  return false;
}

void Interpreter::CallFunction(int line, lFunction* func, std::vector<Literal*> literalVector, Literal* self) {
  //parameter count
  if (func->parameters.size() != literalVector.size()) {
    throw RuntimeError(line, std::string() + "Invalid number of arguments (expected " + std::to_string(func->parameters.size()) + ", got " + std::to_string(literalVector.size()) + ")");
  }

  //create the environment
  Environment* env = new Environment();

  //if calling on a method
  if (self) {
    env->Define(Token(REFERENCE, "this", nullptr, -1), self);
  }

  //recursion
  env->Define(Token(IDENTIFIER, "recurse", nullptr, -1), func);

  //copy into env
  std::vector<std::string>::iterator paramIter = func->parameters.begin();
  std::vector<Literal*>::iterator literalIter = literalVector.begin();

  while(paramIter != func->parameters.end() && literalIter != literalVector.end()) {
    env->Define(Token(IDENTIFIER, *paramIter, *literalIter, -1), *literalIter);
    paramIter++;
    literalIter++;
  }

  //begin execution
  SetResult(nullptr);
  Interpreter* interpreter = new Interpreter(nullptr, env);

  for (Stmt* stmt : static_cast<Block*>(func->block)->stmtVector) {
    interpreter->Execute(stmt);

    //check for a call to return
    if (interpreter->GetReturnCalled()) {
      SetResult( interpreter->GetResult()->Copy() );
      break;
    }
  }

  //CHALLENGE: maintain references returned from functions that would otherwise be lost
  //solution: all return values are deep copies
  //CHALLENGE END

  //finally
  if (self) {
    env->Assign(Token(IDENTIFIER, "this", nullptr, -1), nullptr);
  }
  env->Assign(Token(IDENTIFIER, "recurse", nullptr, -1), nullptr);
  delete interpreter; //also deletes env
}
