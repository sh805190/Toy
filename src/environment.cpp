#include "environment.hpp"

#include "runtime_error.hpp"

Literal Environment::Define(Token name, Literal value) {
  if (literalMap.find(name.GetLexeme()) != literalMap.end()) {
    throw RuntimeError(name.GetLine(), std::string() + "Can't redeclare the variable '" + name.GetLexeme() + "'");
  }

  return literalMap[name.GetLexeme()] = value;
}

Literal Environment::Assign(Token name, Literal value) {
  if (literalMap.find(name.GetLexeme()) == literalMap.end()) {
    throw RuntimeError(name.GetLine(), std::string() + "Undeclared variable '" + name.GetLexeme() + "'");
  }

  return literalMap[name.GetLexeme()] = value;
}

Literal Environment::GetVar(Token name) {
  if (literalMap.find(name.GetLexeme()) == literalMap.end()) {
    throw RuntimeError(name.GetLine(), std::string() + "Undeclared variable '" + name.GetLexeme() + "'");
  }

  return literalMap[name.GetLexeme()];
}

Literal* Environment::GetRef(Token name) {
  if (literalMap.find(name.GetLexeme()) == literalMap.end()) {
    throw RuntimeError(name.GetLine(), std::string() + "Undeclared variable '" + name.GetLexeme() + "'");
  }

  return &literalMap[name.GetLexeme()];
}
