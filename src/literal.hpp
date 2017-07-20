#pragma once

class Literal;
class Array;
class Boolean;
class Class;
class Function;
class Number;
class Object;
class Reference;
class String;
class Undefined;

#include <map>
#include <string>
#include <vector>

class Literal {
public:
  enum class Type {
    ARRAY, BOOLEAN, CLASS, FUNCTION, NUMBER, OBJECT, REFERENCE, STRING, UNDEFINED, 
  };

  Literal() { count++; }
  virtual ~Literal() { count--; }

  virtual Literal* Copy() { return new Literal(); }
  virtual std::string ToString() { return "LITERAL"; }
  Type type;
static int count;
};

class Array: public Literal {
public:
  Array() = default;
  ~Array() = default;

  Array(std::vector<Literal*> array) {
    this->array = std::move(array);
  }

  Literal* Copy() override {
    return new Array(array);
  }

  std::string ToString() override {
    return "array";
  }
  std::vector<Literal*> array;
};

class Boolean: public Literal {
public:
  Boolean() = default;
  ~Boolean() = default;

  Boolean(bool boolean) {
    this->boolean = std::move(boolean);
  }

  Literal* Copy() override {
    return new Boolean(boolean);
  }

  std::string ToString() override {
    return "boolean";
  }
  bool boolean;
};

class Class: public Literal {
public:
  Class() = default;
  ~Class() = default;

  Class(std::map<std::string,Literal*> members) {
    this->members = std::move(members);
  }

  Literal* Copy() override {
    return new Class(members);
  }

  std::string ToString() override {
    return "class";
  }
  std::map<std::string,Literal*> members;
};

class Function: public Literal {
public:
  Function() = default;
  ~Function() = default;

  Function(std::vector<std::string> parameters, void* block) {
    this->parameters = std::move(parameters);
    this->block = std::move(block);
  }

  Literal* Copy() override {
    return new Function(parameters,block);
  }

  std::string ToString() override {
    return "function";
  }
  std::vector<std::string> parameters;
   void* block;
};

class Number: public Literal {
public:
  Number() = default;
  ~Number() = default;

  Number(double number) {
    this->number = std::move(number);
  }

  Literal* Copy() override {
    return new Number(number);
  }

  std::string ToString() override {
    return "number";
  }
  double number;
};

class Object: public Literal {
public:
  Object() = default;
  ~Object() = default;

  Object(std::map<std::string,Literal*> members) {
    this->members = std::move(members);
  }

  Literal* Copy() override {
    return new Object(members);
  }

  std::string ToString() override {
    return "object";
  }
  std::map<std::string,Literal*> members;
};

class Reference: public Literal {
public:
  Reference() = default;
  ~Reference() = default;

  Reference(Literal* reference) {
    this->reference = std::move(reference);
  }

  Literal* Copy() override {
    return new Reference(reference);
  }

  std::string ToString() override {
    return "reference";
  }
  Literal* reference;
};

class String: public Literal {
public:
  String() = default;
  ~String() = default;

  String(std::string str) {
    this->str = std::move(str);
  }

  Literal* Copy() override {
    return new String(str);
  }

  std::string ToString() override {
    return "string";
  }
  std::string str;
};

class Undefined: public Literal {
public:
  Undefined() = default;
  ~Undefined() = default;

  Literal* Copy() override {
    return new Undefined();
  }

  std::string ToString() override {
    return "undefined";
  }
};

