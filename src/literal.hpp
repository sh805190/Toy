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

#include <map>
#include <string>
#include <vector>

class Literal {
public:
  enum class Type {
    LARRAY, LBOOLEAN, LCLASS, LFUNCTION, LNUMBER, LOBJECT, LREFERENCE, LSTRING, LUNDEFINED, 
  };

  Literal() { count++; }
  virtual ~Literal() { count--; }

  virtual Literal* Copy() { return new Literal(); }
  virtual std::string ToString() { return "LITERAL"; }
  Type type;
  //debugging
  static int count;
};

class lArray: public Literal {
public:
  lArray() = default;
  virtual ~lArray() = default;

  lArray(std::vector<Literal*> array) {
    this->array = std::move(array);
  }

  Literal* Copy() override {
    return new lArray(array);
  }

  std::string ToString() override {
    return "larray";
  }
  std::vector<Literal*> array;
};

class lBoolean: public Literal {
public:
  lBoolean() = default;
  virtual ~lBoolean() = default;

  lBoolean(bool boolean) {
    this->boolean = std::move(boolean);
  }

  Literal* Copy() override {
    return new lBoolean(boolean);
  }

  std::string ToString() override {
    return "lboolean";
  }
  bool boolean;
};

class lClass: public Literal {
public:
  lClass() = default;
  virtual ~lClass() = default;

  lClass(std::map<std::string,Literal*> members) {
    this->members = std::move(members);
  }

  Literal* Copy() override {
    return new lClass(members);
  }

  std::string ToString() override {
    return "lclass";
  }
  std::map<std::string,Literal*> members;
};

class lFunction: public Literal {
public:
  lFunction() = default;
  virtual ~lFunction() = default;

  lFunction(std::vector<std::string> parameters, void* block) {
    this->parameters = std::move(parameters);
    this->block = std::move(block);
  }

  Literal* Copy() override {
    return new lFunction(parameters,block);
  }

  std::string ToString() override {
    return "lfunction";
  }
  std::vector<std::string> parameters;
   void* block;
};

class lNumber: public Literal {
public:
  lNumber() = default;
  virtual ~lNumber() = default;

  lNumber(double number) {
    this->number = std::move(number);
  }

  Literal* Copy() override {
    return new lNumber(number);
  }

  std::string ToString() override {
    return "lnumber";
  }
  double number;
};

class lObject: public Literal {
public:
  lObject() = default;
  virtual ~lObject() = default;

  lObject(std::map<std::string,Literal*> members) {
    this->members = std::move(members);
  }

  Literal* Copy() override {
    return new lObject(members);
  }

  std::string ToString() override {
    return "lobject";
  }
  std::map<std::string,Literal*> members;
};

class lReference: public Literal {
public:
  lReference() = default;
  virtual ~lReference() = default;

  lReference(Literal* reference) {
    this->reference = std::move(reference);
  }

  Literal* Copy() override {
    return new lReference(reference);
  }

  std::string ToString() override {
    return "lreference";
  }
  Literal* reference;
};

class lString: public Literal {
public:
  lString() = default;
  virtual ~lString() = default;

  lString(std::string str) {
    this->str = std::move(str);
  }

  Literal* Copy() override {
    return new lString(str);
  }

  std::string ToString() override {
    return "lstring";
  }
  std::string str;
};

class lUndefined: public Literal {
public:
  lUndefined() = default;
  virtual ~lUndefined() = default;

  Literal* Copy() override {
    return new lUndefined();
  }

  std::string ToString() override {
    return "lundefined";
  }
};

