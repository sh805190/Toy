#pragma once

#include <string>

class Literal {
public:
  enum class Type {
    BOOLEAN, NUMBER, REFERENCE, STRING, UNDEFINED
  };

  Literal();
  Literal(Type);
  Literal(bool);
  Literal(double);
  Literal(Literal*);
  Literal(std::string);

  Literal& operator=(const Literal&);

  Type GetType();

  bool SetBoolean(bool);
  bool GetBoolean();

  double SetNumber(double);
  double GetNumber();

  Literal* SetReference(Literal*);
  Literal* GetReference();

  std::string SetString(std::string);
  std::string GetString();

  //debugging
  std::string ToString();

private:
  Type type;
  bool boolean = false;
  double number = 0;
  Literal* reference = nullptr;
  std::string str;
};
