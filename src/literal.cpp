#include "literal.hpp"

#include <assert.h>

Literal::Literal(): type(Type::UNDEFINED) {
  //EMPTY
}

Literal::Literal(Type t): type(t) {
  assert(type != Type::UNDEFINED);
}

Literal::Literal(bool b): type(Type::BOOLEAN) {
  boolean = b;
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

Literal& Literal::operator=(const Literal& rhs) {
  this->type = rhs.type;
  this->boolean = rhs.boolean;
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
