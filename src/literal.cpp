#include "literal.hpp"

#include <assert.h>

Literal::Literal(Type t): type(t) {
  //EMPTY
}

Literal::Literal(double d): type(Literal::Type::NUMBER) {
  number = d;
}

Literal::Literal(std::string s): type(Literal::Type::STRING) {
  str = s;
}

Literal& Literal::operator=(const Literal& rhs) {
  this->type = rhs.type;
  this->number = rhs.number;
  this->str = rhs.str;
  return *this;
}

Literal::Type Literal::GetType() {
  return type;
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
    case Type::NUMBER:
      return std::to_string(number);
    case Type::STRING:
      return str;
  }
}
