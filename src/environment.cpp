#include "environment.hpp"

#include "runtime_error.hpp"

Environment::Environment(Environment* parent) {
  this->parent = parent;
}

Environment::~Environment() {
  //clean up the literals
  for (auto p : literalMap) {
    delete p.second;
  }
}

Literal* Environment::Define(Token name, Literal* value) {
  //TODO: Does the name really need to be a token?
  if (literalMap.find(name.GetLexeme()) != literalMap.end()) {
    throw RuntimeError(name.GetLine(), std::string() + "Can't redeclare the variable '" + name.GetLexeme() + "'");
  }

  return literalMap[name.GetLexeme()] = value ? value->Copy() : new lUndefined();
}

Literal* Environment::Assign(Token name, Literal* value) {
  if (literalMap.find(name.GetLexeme()) == literalMap.end()) {
    if (parent) {
      return parent->Assign(name, value);
    }
    throw RuntimeError(name.GetLine(), std::string() + "Undeclared variable '" + name.GetLexeme() + "' in assignment");
  }

  delete literalMap[name.GetLexeme()];
  return literalMap[name.GetLexeme()] = value ? value->Copy() : new lUndefined();
}

Literal* Environment::GetVar(Token name) {
  //GetVar only returns a copy
  return GetRef(name)->Copy();
}

Literal* Environment::GetRef(Token name) {
  if (literalMap.find(name.GetLexeme()) == literalMap.end()) {
    if (parent) {
      return parent->GetRef(name);
    }
    throw RuntimeError(name.GetLine(), std::string() + "Undeclared variable '" + name.GetLexeme() + "' found");
  }

  return literalMap[name.GetLexeme()];
}

void Environment::Absorb(Environment* env) {
  for (auto p : env->literalMap) {
    Define(Token(IDENTIFIER, p.first, nullptr, -1), p.second);
  }
}

Environment* Environment::GetParent() {
  return parent;
}

std::map<std::string, Literal*>* Environment::GetLiteralMap() {
  return &literalMap;
}
