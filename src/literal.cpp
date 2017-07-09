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

Literal::Literal(std::string s): type(Type::STRING) {
  str = s;
}

Literal& Literal::operator=(const Literal& rhs) {
  this->type = rhs.type;
  this->boolean = rhs.boolean;
  this->number = rhs.number;
  this->str = rhs.str;
  return *this;
}

Literal::Type Literal::GetType() {
  return type;
}

bool Literal::SetBoolean(bool b) {
  assert(type == Type::BOOLEAN);
  return boolean = b;
}

bool Literal::GetBoolean() {
  assert(type == Type::BOOLEAN);
  return boolean;
}

double Literal::SetNumber(double d) {
  assert(type == Type::NUMBER);
  return number = d;
}

double Literal::GetNumber() {
  assert(type == Type::NUMBER);
  return number;
}

std::string Literal::SetString(std::string s) {
  assert(type == Type::STRING);
  return str = s;
}

std::string Literal::GetString() {
  assert(type == Type::STRING);
  return str;
}

std::string Literal::ToString() {
  switch(type) {
    case Type::BOOLEAN:
      return boolean ? "true" : "false";
    case Type::NUMBER:
      return std::to_string(number);
    case Type::STRING:
      return str;
  }
}
