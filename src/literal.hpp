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

#include "garbage_collector.hpp"

#include <map>
#include <string>
#include <vector>

class Literal {
public:
  enum class Type {
    LARRAY, LBOOLEAN, LCLASS, LFUNCTION, LNUMBER, LOBJECT, LREFERENCE, LSTRING, LUNDEFINED, 
  };

  Literal() { count++; GarbageCollector<Literal>::Push(this); }
  virtual ~Literal() { count--; GarbageCollector<Literal>::Pop(this); }

  virtual Literal* Copy() {
    Literal* l = new Literal();
    l->type = type;
    return l;
  }

  virtual std::string ToString() {
    return "LITERAL";
  }

  Type type = Type::LUNDEFINED;
  //debugging
  static int count;
};

class lArray: public Literal {
public:
  lArray() { type = Type::LARRAY; }
  virtual ~lArray() = default;

  lArray(std::vector<Literal*> array) {
    type = Type::LARRAY;
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
  lBoolean() { type = Type::LBOOLEAN; }
  virtual ~lBoolean() = default;

  lBoolean(bool boolean) {
    type = Type::LBOOLEAN;
    this->boolean = std::move(boolean);
  }

  Literal* Copy() override {
    Literal* l = new lBoolean(boolean);
    l->type = type;
    return l;
  }

  std::string ToString() override {
    return boolean ? "true" : "false";
  }

  bool boolean;
};

class lClass: public Literal {
public:
  lClass() { type = Type::LCLASS; }
  virtual ~lClass() = default;

  lClass(std::map<std::string,Literal*> members) {
    type = Type::LCLASS;
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
  lFunction() { type = Type::LFUNCTION; }
  virtual ~lFunction() = default;

  lFunction(std::vector<std::string> parameters, void* block) {
    type = Type::LFUNCTION;
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
  lNumber() { type = Type::LNUMBER; }
  virtual ~lNumber() = default;

  lNumber(double number) {
    type = Type::LNUMBER;
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
  lObject() { type = Type::LOBJECT; }
  virtual ~lObject() = default;

  lObject(std::map<std::string,Literal*> members) {
    type = Type::LOBJECT;
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
  lReference() { type = Type::LREFERENCE; }
  virtual ~lReference() = default;

  lReference(Literal* reference) {
    type = Type::LREFERENCE;
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
  lString() { type = Type::LSTRING; }
  virtual ~lString() = default;

  lString(std::string str) {
    type = Type::LSTRING;
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
  lUndefined() { type = Type::LUNDEFINED; }
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

