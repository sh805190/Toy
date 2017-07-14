#pragma once

#include <list>
#include <string>

class Literal {
public:
  enum class Type {
    BOOLEAN, FUNCTION, NUMBER, REFERENCE, STRING, UNDEFINED
  };

  Literal();
  Literal(const Literal&);
  Literal(Type);
  Literal(bool);
  Literal(std::list<std::string>& varList, void* block);
  Literal(double);
  Literal(Literal*);
  Literal(std::string);

  ~Literal();

  Literal& operator=(const Literal&);

  Type GetType();

  bool SetBoolean(bool);
  bool GetBoolean();

  std::list<std::string> SetVarList(std::list<std::string> sl);
  std::list<std::string> GetVarList();

  void* SetBlock(void*);
  void* GetBlock();

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
  std::list<std::string> varList;
  void* block = nullptr; //using Block type caused issues
  double number = 0;
  Literal* reference = nullptr;
  std::string str;
};
