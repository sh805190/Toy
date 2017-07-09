#pragma once

#include <string>

class Literal {
public:
  enum class Type {
    BOOLEAN, NUMBER, STRING, UNDEFINED
  };

  Literal();
  Literal(Type);
  Literal(bool);
  Literal(double);
  Literal(std::string);

  Literal& operator=(const Literal&);

  Type GetType();

  bool SetBoolean(bool);
  bool GetBoolean();

  double SetNumber(double);
  double GetNumber();

  std::string SetString(std::string);
  std::string GetString();

  //debugging
  std::string ToString();

private:
  Type type;
  bool boolean = false;
  double number = 0;
  std::string str;
};
