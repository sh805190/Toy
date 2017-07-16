#pragma once

#include <string>
#include <vector>

class Literal {
public:
  enum class Type {
    ARRAY, BOOLEAN, FUNCTION, NUMBER, REFERENCE, STRING, UNDEFINED
  };

  Literal();
  Literal(const Literal&);
  Literal(Type);
  Literal(std::vector<Literal>& literalVector);
  Literal(bool);
  Literal(std::vector<std::string>& parameterVector, void* block);
  Literal(double);
  Literal(Literal*);
  Literal(std::string);

  ~Literal();

  Literal& operator=(const Literal&);

  Type GetType();

  std::vector<Literal> SetLiteralVector(std::vector<Literal>& literalVector);
  std::vector<Literal> GetLiteralVector();

  bool SetBoolean(bool);
  bool GetBoolean();

  std::vector<std::string> SetParameterVector(std::vector<std::string>& parameterVector);
  std::vector<std::string> GetParameterVector();

  void* SetBlock(void*);
  void* GetBlock();

  double SetNumber(double);
  double GetNumber();

  Literal* SetReference(Literal*);
  Literal* GetReference();

  std::string SetString(std::string);
  std::string GetString();

  std::string ToString();

private:
  Type type;
  std::vector<Literal> literalVector;
  bool boolean = false;
  std::vector<std::string> parameterVector;
  void* block = nullptr; //using Block type caused issues
  double number = 0;
  Literal* reference = nullptr;
  std::string str;
};
