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

  virtual Literal* Copy() {
    Literal* l = new Literal();
    l->type = type;
    return l;
  }

  virtual std::string ToString() {
    return "LITERAL";
  }

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
    Literal* l = new lArray(array);
    l->type = type;
    return l;
  }

  std::string ToString() override {
    std::string output = "[";for (Literal* ptr : array){output += ptr->ToString();output += ",";}return output + "]";
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
    Literal* l = new lBoolean(boolean);
    l->type = type;
    return l;
  }

  std::string ToString() override {
    return std::to_string(boolean);
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
    Literal* l = new lClass(members);
    l->type = type;
    return l;
  }

  std::string ToString() override {
    std::string output = "class {";for (auto& it : members) {output += it.first + ":";output += it.second->ToString();output += ",";}return output + "}";
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
    Literal* l = new lFunction(parameters,block);
    l->type = type;
    return l;
  }

  std::string ToString() override {
    std::string output = "function(";for (std::string s : parameters) {output += s;output += ",";}return output + ") {...}";
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
    Literal* l = new lNumber(number);
    l->type = type;
    return l;
  }

  std::string ToString() override {
    return std::to_string(number);
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
    Literal* l = new lObject(members);
    l->type = type;
    return l;
  }

  std::string ToString() override {
    std::string output = "object {"; for (auto& it : members) {output += it.first; output += ",";}return output + "}";
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
    Literal* l = new lReference(reference);
    l->type = type;
    return l;
  }

  std::string ToString() override {
    return std::string() + "&" + reference->ToString();
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
    Literal* l = new lString(str);
    l->type = type;
    return l;
  }

  std::string ToString() override {
    return str;
  }

  std::string str;
};

class lUndefined: public Literal {
public:
  lUndefined() = default;
  virtual ~lUndefined() = default;

  Literal* Copy() override {
    Literal* l = new lUndefined();
    l->type = type;
    return l;
  }

  std::string ToString() override {
    return "undefined";
  }

};

