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

Literal::Literal(std::vector<Literal>& lv): type(Type::ARRAY) {
  literalVector = lv;
}

Literal::Literal(bool b): type(Type::BOOLEAN) {
  boolean = b;
}

Literal::Literal(std::vector<std::string>& sv, void* b): type(Type::FUNCTION) {
  ASTDuplicator duplicator;

  parameterVector = sv;
  block = reinterpret_cast<void*>(duplicator.DuplicateAST(reinterpret_cast<Block*>(b)));
}

Literal::Literal(Environment* env): type(Type::CLASS) {
  literalMap = *(env->GetLiteralMapRef());
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
  if (type == Type::CLASS || type == Type::FUNCTION) {
    ASTDeleter deleter;
    deleter.DeleteAST(reinterpret_cast<Block*>(block));
  }
}

Literal& Literal::operator=(const Literal& rhs) {
  ASTDeleter deleter;
  ASTDuplicator duplicator;

  this->type = rhs.type;
  this->literalVector = rhs.literalVector;
  this->boolean = rhs.boolean;
  this->parameterVector = rhs.parameterVector;
  deleter.DeleteAST(reinterpret_cast<Block*>(this->block));
  this->block = reinterpret_cast<void*>(duplicator.DuplicateAST(reinterpret_cast<Block*>(rhs.block)));
  this->number = rhs.number;
  this->reference = rhs.reference;
  this->str = rhs.str;

  return *this;
}

Literal::Type Literal::SetType(Type t) {
  return type = t;
}

Literal::Type Literal::GetType() {
  return type;
}

std::vector<Literal> Literal::SetLiteralVector(std::vector<Literal>& lv) {
  type = Type::ARRAY;
  return literalVector = lv;
}

std::vector<Literal> Literal::GetLiteralVector() {
  if (type == Type::REFERENCE) {
    return reference->GetLiteralVector();
  }

  assert(type == Type::ARRAY);
  return literalVector;
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

std::vector<std::string> Literal::SetParameterVector(std::vector<std::string>& pv) {
  type = Type::FUNCTION;
  return parameterVector = pv;
}

std::vector<std::string> Literal::GetParameterVector() {
  if (type == Type::REFERENCE) {
    return reference->GetParameterVector();
  }

  assert(type == Type::FUNCTION);
  return parameterVector;
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

  assert(type == Type::CLASS || type == Type::FUNCTION);
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

Literal Literal::SetMember(std::string key, Literal val) {
  if (type == Type::REFERENCE) {
    return reference->SetMember(key, val);
  }
  type = Type::OBJECT;
  return literalMap[key] = val;
}

Literal Literal::GetMember(std::string key) {
  assert(type == Type::OBJECT);
  return literalMap[key];
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
  //ARRAY, BOOLEAN, CLASS, FUNCTION, NUMBER, OBJECT, REFERENCE, STRING, UNDEFINED
  switch(type) {
    case Type::ARRAY: {
      std::string output = "[";
      for (auto& it : literalVector) {
        output += it.ToString();
        output += ",";
      }
      if (literalVector.size()) output.erase(output.end()-1);
      output += "]";
      return output;
    }

    case Type::BOOLEAN:
      return boolean ? "true" : "false";

    case Type::CLASS:
      return "class {...}";

    case Type::FUNCTION: {
      std::string output = "function(";
      for (auto& it : parameterVector) {
        output += it;
        output += ",";
      }
      //prune the last comma
      if (parameterVector.size()) output.erase(output.end()-1);
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

    case Type::OBJECT: {
      std::string output = "object {";
      for (auto& it : literalMap) {
        output += it.first;
        output += ",";
      }
      //prune the last comma
      if (literalMap.size()) output.erase(output.end()-1);
      output += "};";
      return output;
    }

    case Type::REFERENCE:
      return std::string() + "&" + reference->ToString();

    case Type::STRING:
      return str;

    default:
      return "undefined";
  }
}
