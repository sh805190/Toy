#include "literal.hpp"

#include "ast_deleter.hpp"
#include "ast_duplicator.hpp"
#include "stmt.hpp"

#include <assert.h>

Literal::Literal(): type(Type::UNDEFINED) {
  //EMPTY
}

Literal::Literal(const Literal& rhs) {
  *this = rhs;
}

Literal::Literal(Type t): type(t) {
  assert(type != Type::UNDEFINED);
}

Literal::Literal(bool b): type(Type::BOOLEAN) {
  boolean = b;
}

Literal::Literal(std::list<std::string>& sl, void* b): type(Type::FUNCTION) {
  ASTDuplicator duplicator;

  varList = sl;
  block = reinterpret_cast<void*>(duplicator.DuplicateAST(reinterpret_cast<Block*>(b)));
}

Literal::Literal(double d): type(Type::NUMBER) {
  number = d;
}

Literal::Literal(Literal* rhs): type(Type::REFERENCE) {
  reference = rhs;
}

Literal::Literal(std::string s): type(Type::STRING) {
  str = s;
}

Literal::~Literal() {
  if (type == Type::FUNCTION) {
    ASTDeleter deleter;
    deleter.DeleteAST(reinterpret_cast<Block*>(block));
  }
}

Literal& Literal::operator=(const Literal& rhs) {
  ASTDeleter deleter;
  ASTDuplicator duplicator;

  this->type = rhs.type;
  this->boolean = rhs.boolean;
  this->varList = rhs.varList;
  deleter.DeleteAST(reinterpret_cast<Block*>(this->block));
  this->block = reinterpret_cast<void*>(duplicator.DuplicateAST(reinterpret_cast<Block*>(rhs.block)));
  this->number = rhs.number;
  this->reference = rhs.reference;
  this->str = rhs.str;
  return *this;
}

Literal::Type Literal::GetType() {
  return type;
}

bool Literal::SetBoolean(bool b) {
  type = Type::BOOLEAN;
  return boolean = b;
}

bool Literal::GetBoolean() {
  if (type == Type::REFERENCE) {
    return reference->GetBoolean();
  }

  assert(type == Type::BOOLEAN);
  return boolean;
}

std::list<std::string> Literal::SetVarList(std::list<std::string> sl) {
  type = Type::FUNCTION;
  return varList = sl;
}

std::list<std::string> Literal::GetVarList() {
  if (type == Type::REFERENCE) {
    return reference->GetVarList();
  }

  return varList;
}

void* Literal::SetBlock(void* b) {
  ASTDeleter deleter;
  ASTDuplicator duplicator;

  type = Type::FUNCTION;

  deleter.DeleteAST(reinterpret_cast<Block*>(block));  
  block = reinterpret_cast<void*>(duplicator.DuplicateAST(reinterpret_cast<Block*>(b)));
}

void* Literal::GetBlock() {
  if (type == Type::REFERENCE) {
    return reference->GetBlock();
  }

  return block;
}

double Literal::SetNumber(double d) {
  type = Type::NUMBER;
  return number = d;
}

double Literal::GetNumber() {
  if (type == Type::REFERENCE) {
    return reference->GetNumber();
  }

  assert(type == Type::NUMBER);
  return number;
}

Literal* Literal::SetReference(Literal* rhs) {
  type = Type::REFERENCE;
  return reference = rhs;
}

Literal* Literal::GetReference() {
  assert(type == Type::REFERENCE);
  return reference;
}

std::string Literal::SetString(std::string s) {
  type = Type::STRING;
  return str = s;
}

std::string Literal::GetString() {
  if (type == Type::REFERENCE) {
    return reference->GetString();
  }

  assert(type == Type::STRING);
  return str;
}

std::string Literal::ToString() {
  switch(type) {
    case Type::BOOLEAN:
      return boolean ? "true" : "false";

    case Type::FUNCTION: {
      bool prune = false;
      std::string output = "function(";
      for (std::string arg : varList) {
        output += arg;
        output += ",";
        prune = true;
      }
      //prune the last comma
      if (prune) output.erase(output.end()-1);
      output += ") {...}";
      return output;
    }

    case Type::NUMBER: {
      std::string s = std::to_string(number);
      s = s.substr(0, s.find_last_not_of('0') + 1);
      if (s[s.size()-1] == '.') {
        s = s.substr(0, s.size() - 1);
      }
      return s;
    }

    case Type::REFERENCE:
      return reference->ToString();

    case Type::STRING:
      return str;

    default:
      return "undefined";
  }
}
