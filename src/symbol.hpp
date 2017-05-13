#pragma once

#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

//forward declarations
class Symbol;

//symbolType
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
  COMMA,
  PIPE,

  COMMENT,
  SYMBOL_LAST
};

//symbol constants
const std::map<symbolType, const char*> symbolConst = {
  //var types
  {ARRAY, ""},
  {FUNCTION, ""},
  {NUMBER, ""},
  {REFERENCE, ""},
  {STRING, ""},

  //reserved keywords
  {BREAK, "break"},
  {CASE, "case"},
  {CONTINUE, "continue"},
  {DEFAULT, "default"},
  {ELSE, "else"},
  {FOR, "for"},
  {GOTO, "goto"},
  {IF, "if"},
  {MAIN, "main"},
  {MODULE, "module"},
  {PRELOAD, "preload"},
  {RETURN, "return"},
  {SWITCH, "switch"},
  {UNDEFINED, "undefined"},
  {USE, "use"},
  {VAR, "var"},
  {WHILE, "while"},

  //operators
  {PARENTHESIS_LEFT, "("},
  {PARENTHESIS_RIGHT, ")"},
  {ASSIGNMENT, "="},
  {ADDITION, "+"},
  {SUBTRACTION, "-"},
  {MULTIPLICATION, "*"},
  {DIVISION, "/"},
  {MODULUS, "%"},
  {COMPARITOR, "=="},
  {NOT, "!"},
  {AND, "&&"},
  {OR, "||"},
  {LESS_THAN, "<"},
  {GREATER_THAN, ">"},
  {LESS_THAN_OR_EQUAL, "<="},
  {GREATER_THAN_OR_EQUAL, ">="},
  {BRACE_LEFT, "{"},
  {BRACE_RIGHT, "}"},
  {BRACKET_LEFT, "["},
  {BRACKET_RIGHT, "]"},
  {REFERENCE_MARK, "&"},
  {CONCATENATION, "."},
  {SEPARATOR, ":"},
  {QUOTE, "\""},
  {COMMA, ","},
  {PIPE, "|"},

  {COMMENT, "//"}
};

//symbol value, stored as a shared pointer
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
    //TODO: functions
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

//the symbol struct proper
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

//symbol functions
std::string toString(Symbol symbol);
