#pragma once

#include <list>
#include <memory>
#include <string>
#include <vector>

//forward declarations
class Symbol;

enum symbolType {
  //var types
  ARRAY,
  FUNCTION,
  NUMBER,
  REFERENCE,
  STRING,

  //reserved keywords
  BREAK,
  CASE,
  CONTINUE,
  DEFAULT,
  ELSE,
  FOR,
  GOTO,
  IF,
  MAIN,
  MODULE,
  PRELOAD,
  RETURN,
  SWITCH,
  UNDEFINED,
  USE,
  VAR,
  WHILE,

  //operators
  PARENTHESIS_LEFT,
  PARENTHESIS_RIGHT,
  ASSIGNMENT,
  ADDITION,
  SUBTRACTION,
  MULTIPLICATION,
  DIVISION,
  MODULUS,
  COMPARITOR,
  NOT,
  AND,
  OR,
  LESS_THAN,
  GREATER_THAN,
  LESS_THAN_OR_EQUAL,
  GREATER_THAN_OR_EQUAL,
  BRACE_LEFT,
  BRACE_RIGHT,
  BRACKET_LEFT,
  BRACKET_RIGHT,
  REFERENCE_MARK,
  CONCATENATION,
  SEPARATOR,
  QUOTE,
  COMMA
};

struct symbolValue {
  symbolValue& operator=(double d) {
    number = d;
    return *this;
  }

  symbolValue& operator=(std::string s) {
    string = s;
    return *this;
  }

  symbolValue& operator=(std::initializer_list<Symbol> il) {
    array = il;
    return *this;
  }

  symbolValue& operator=(Symbol* p) {
    reference = std::make_shared<Symbol>(*p);
    return *this;
  }

  double number;
  std::string string;
  std::vector<Symbol> array;
  std::list<Symbol> function;
  std::shared_ptr<Symbol> reference;
};

struct Symbol {
  Symbol() {
    value = std::make_shared<symbolValue>();
  }
  Symbol(symbolType t):Symbol() {
    type = t;
  }

  symbolType type;
  std::shared_ptr<symbolValue> value;
};
