#include "environment.hpp"

#include "runtime_error.hpp"

Environment::Environment(Environment* parent) {
  this->parent = parent;
}

Literal Environment::Define(Token name, Literal value) {
  if (literalMap.find(name.GetLexeme()) != literalMap.end()) {
    throw RuntimeError(name.GetLine(), std::string() + "Can't redeclare the variable '" + name.GetLexeme() + "'");
  }

  return literalMap[name.GetLexeme()] = value;
}

Literal Environment::Assign(Token name, Literal value) {
  if (literalMap.find(name.GetLexeme()) == literalMap.end()) {
    if (parent) {
      return parent->Assign(name, value);
    }
    throw RuntimeError(name.GetLine(), std::string() + "Undeclared variable '" + name.GetLexeme() + "' in assignment");
  }

  return literalMap[name.GetLexeme()] = value;
}

Literal Environment::GetVar(Token name) {
  if (literalMap.find(name.GetLexeme()) == literalMap.end()) {
    if (parent) {
      return parent->GetVar(name);
    }
    throw RuntimeError(name.GetLine(), std::string() + "Undeclared variable '" + name.GetLexeme() + "' found");
  }

  return literalMap[name.GetLexeme()];
}

Literal* Environment::GetRef(Token name) {
  if (literalMap.find(name.GetLexeme()) == literalMap.end()) {
    if (parent) {
      return parent->GetRef(name);
    }
    throw RuntimeError(name.GetLine(), std::string() + "Undeclared variable '" + name.GetLexeme() + "' found when retreiving reference");
  }

  return &literalMap[name.GetLexeme()];
}

Environment* Environment::GetParent() {
  return parent;
}

std::map<std::string, Literal>* Environment::GetLiteralMapRef() {
  return &literalMap;
}
