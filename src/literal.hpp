#pragma once

#include <string>

class Literal {
public:
  enum class Type {
    NUMBER, STRING
  };

  Literal();
  Literal(Type);
  Literal(double);
  Literal(std::string);

  Type GetType();

  double SetNumber(double);
  double GetNumber();

  std::string SetString(std::string);
  std::string GetString();

  //debugging
  std::string ToString();

private:
  const Type type;
  double number = 0;
  std::string str;
};
