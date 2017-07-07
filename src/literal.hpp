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

  Literal& operator=(const Literal&);

  Type GetType();

  double SetNumber(double);
  double GetNumber();

  std::string SetString(std::string);
  std::string GetString();

  //debugging
  std::string ToString();

private:
  Type type;
  double number = 0;
  std::string str;
};
