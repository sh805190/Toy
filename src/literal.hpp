#pragma once

class Literal;
class lArray;
class lBoolean;
class lClass;
class lFunction;
class lNumber;
class lObject;
class lReference;
class lString;
class lUndefined;

#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"

#include <map>
#include <string>
#include <vector>

class Literal {
public:
  enum class Type {
    LARRAY, LBOOLEAN, LCLASS, LFUNCTION, LNUMBER, LOBJECT, LREFERENCE, LSTRING, LUNDEFINED, 
  };

  Literal();
  virtual ~Literal();
  virtual Literal* Copy();
  virtual std::string ToString();

  Type type = Type::LUNDEFINED;
  //debugging
  static int count;
};

class lArray: public Literal {
public:
  lArray();
  lArray(std::vector<Literal*> array);
  virtual ~lArray();
  Literal* Copy() override;
  std::string ToString() override;

  std::vector<Literal*> array;
};

class lBoolean: public Literal {
public:
  lBoolean();
  lBoolean(bool boolean);
  virtual ~lBoolean();
  Literal* Copy() override;
  std::string ToString() override;

  bool boolean;
};

class lClass: public Literal {
public:
  lClass();
  lClass(std::map<std::string,Literal*> members);
  virtual ~lClass();
  Literal* Copy() override;
  std::string ToString() override;

  std::map<std::string,Literal*> members;
};

class lFunction: public Literal {
public:
  lFunction();
  lFunction(std::vector<std::string> parameters, void* block);
  virtual ~lFunction();
  Literal* Copy() override;
  std::string ToString() override;

  std::vector<std::string> parameters;
   void* block;
};

class lNumber: public Literal {
public:
  lNumber();
  lNumber(double number);
  virtual ~lNumber();
  Literal* Copy() override;
  std::string ToString() override;

  double number;
};

class lObject: public Literal {
public:
  lObject();
  lObject(std::map<std::string,Literal*> members);
  virtual ~lObject();
  Literal* Copy() override;
  std::string ToString() override;

  std::map<std::string,Literal*> members;
};

class lReference: public Literal {
public:
  lReference();
  lReference(Literal* reference);
  virtual ~lReference();
  Literal* Copy() override;
  std::string ToString() override;

  Literal* reference;
};

class lString: public Literal {
public:
  lString();
  lString(std::string str);
  virtual ~lString();
  Literal* Copy() override;
  std::string ToString() override;

  std::string str;
};

class lUndefined: public Literal {
public:
  lUndefined();
  virtual ~lUndefined();
  Literal* Copy() override;
  std::string ToString() override;
};

