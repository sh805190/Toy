#pragma once

#include "literal.hpp"
#include "token.hpp"

#include <map>
#include <string>

class Environment {
public:
  Environment() = default;
  Environment(Environment* parent);

  Literal Define(Token name, Literal value);
  Literal Assign(Token name, Literal value);
  Literal GetVar(Token name);
  Literal* GetRef(Token name);

  Environment* GetParent();

private:
  std::map<std::string, Literal> literalMap;
  Environment* parent = nullptr;
};
