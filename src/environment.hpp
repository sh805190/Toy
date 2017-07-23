#pragma once

#include "token.hpp"
#include "literal.hpp"

#include <map>
#include <string>

class Environment {
public:
  Environment() = default;
  Environment(Environment* parent);

  Literal* Define(Token name, Literal* value);
  Literal* Assign(Token name, Literal* value);
  Literal* GetVar(Token name);
  Literal* GetRef(Token name);

  Environment* GetParent();

  std::map<std::string, Literal*>* GetLiteralMap();

private:
  std::map<std::string, Literal*> literalMap;
  Environment* parent = nullptr;
};
