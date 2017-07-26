#include "literal.hpp"

int Literal::count = 0;

#include "garbage_collector.hpp"
#include "ast_deleter.hpp"
#include "ast_duplicator.hpp"
#include "literal.hpp"

Literal::Literal() {
  count++;
  GarbageCollector<Literal>::Push(this);
}

Literal::~Literal() {
  count--;
  GarbageCollector<Literal>::Pop(this);
}

Literal* Literal::Copy() {
  Literal* l = new Literal();
  l->type = type;
  return l;
}

std::string Literal::ToString() {
  return "LITERAL";
}

lArray::lArray() {
  type = Type::LARRAY;
}

lArray::lArray(std::vector<Literal*> array) {
  type = Type::LARRAY;
  this->array = std::move(array);
}

lArray::~lArray() {
  for (Literal* ptr : array) delete ptr;
}

Literal* lArray::Copy() {
  lArray* ret = new lArray(); for (Literal* ptr : array) ret->array.push_back(ptr->Copy()); return ret;
}

std::string lArray::ToString() {
  std::string output = "[";for (Literal* ptr : array){output += ptr->ToString();output += ",";}return output + "]";
}

lBoolean::lBoolean() {
  type = Type::LBOOLEAN;
}

lBoolean::lBoolean(bool boolean) {
  type = Type::LBOOLEAN;
  this->boolean = std::move(boolean);
}

lBoolean::~lBoolean() {
  ;
}

Literal* lBoolean::Copy() {
  return new lBoolean(boolean);
}

std::string lBoolean::ToString() {
  return boolean ? "true" : "false";
}

lClass::lClass() {
  type = Type::LCLASS;
}

lClass::lClass(std::map<std::string,Literal*> members) {
  type = Type::LCLASS;
  this->members = std::move(members);
}

lClass::~lClass() {
  for (auto p : members) delete p.second;
}

Literal* lClass::Copy() {
  lClass* ret = new lClass(); for (auto p : members) ret->members[p.first] = p.second->Copy(); return ret;
}

std::string lClass::ToString() {
  std::string output = "class {";for (auto& it : members) {output += it.first + ":";output += it.second->ToString();output += ",";}return output + "}";
}

lFunction::lFunction() {
  type = Type::LFUNCTION;
}

lFunction::lFunction(std::vector<std::string> parameters, void* block) {
  type = Type::LFUNCTION;
  this->parameters = std::move(parameters);
  this->block = std::move(block);
}

lFunction::~lFunction() {
  ASTDeleter deleter; deleter.DeleteAST(static_cast<Block*>(block));
}

Literal* lFunction::Copy() {
  ASTDuplicator dup; lFunction* ret = new lFunction(); ret->parameters = parameters; ret->block = static_cast<void*>(dup.DuplicateAST(static_cast<Block*>(block))); return ret;
}

std::string lFunction::ToString() {
  std::string output = "function(";for (std::string s : parameters) {output += s;output += ",";}return output + ") {...}";
}

lNumber::lNumber() {
  type = Type::LNUMBER;
}

lNumber::lNumber(double number) {
  type = Type::LNUMBER;
  this->number = std::move(number);
}

lNumber::~lNumber() {
  ;
}

Literal* lNumber::Copy() {
  return new lNumber(number);
}

std::string lNumber::ToString() {
  std::string s = std::to_string(number); s = s.substr(0, s.find_last_not_of('0') + 1); if (s[s.size()-1] == '.') s = s.substr(0, s.size()-1); return s;
}

lObject::lObject() {
  type = Type::LOBJECT;
}

lObject::lObject(std::map<std::string,Literal*> members) {
  type = Type::LOBJECT;
  this->members = std::move(members);
}

lObject::~lObject() {
  for (auto p : members) delete p.second;
}

Literal* lObject::Copy() {
  lObject* ret = new lObject(); for (auto p : members) ret->members[p.first] = p.second->Copy(); return ret;
}

std::string lObject::ToString() {
  std::string output = "object {"; for (auto& it : members) {output += it.first; output += ",";}return output + "}";
}

lReference::lReference() {
  type = Type::LREFERENCE;
}

lReference::lReference(Literal* reference) {
  type = Type::LREFERENCE;
  this->reference = std::move(reference);
}

lReference::~lReference() {
  ;
}

Literal* lReference::Copy() {
  return new lReference(reference);
}

std::string lReference::ToString() {
  return std::string() + "&" + reference->ToString();
}

lString::lString() {
  type = Type::LSTRING;
}

lString::lString(std::string str) {
  type = Type::LSTRING;
  this->str = std::move(str);
}

lString::~lString() {
  ;
}

Literal* lString::Copy() {
  return new lString(str);
}

std::string lString::ToString() {
  return str;
}

lUndefined::lUndefined() {
  type = Type::LUNDEFINED;
}

lUndefined::~lUndefined() {
  ;
}

Literal* lUndefined::Copy() {
  return new lUndefined();
}

std::string lUndefined::ToString() {
  return "undefined";
}

