#include "symbol.hpp"

#include <iostream>

std::string toString(Symbol);
Symbol parseString(std::string);

int main(int argc, char* argv[]) {
  //test array
  {
    Symbol symbol;
    symbol.type = symbolType::ARRAY;

    //build the elements of the array
    Symbol member1(symbolType::NUMBER);
    Symbol member2(symbolType::NUMBER);
    Symbol member3(symbolType::NUMBER);
    Symbol member4(symbolType::NUMBER);
    Symbol member5(symbolType::NUMBER);

    *member1.value = 1;
    *member2.value = 2;
    *member3.value = 3;
    *member4.value = 4;
    *member5.value = 5;

    *symbol.value = {member1, member2, member3, member4, member5};

    std::cout << "array:\t" << toString(symbol) << std::endl;
  }

  //test function
  {
    Symbol symbol;

    symbol.type = symbolType::FUNCTION;
    //TODO: set a function

    std::cout << "func:\t" << toString(symbol) << std::endl;
  }

  //test number
  {
    Symbol symbol(symbolType::NUMBER);
    *symbol.value = 42;
    std::cout << "number:\t" << toString(symbol) << std::endl;
  }

  //test string
  {
    Symbol symbol(symbolType::STRING);
    *symbol.value = "foobar";
    std::cout << "string:\t" << toString(symbol) << std::endl;
  }

  //test reference
  {
    Symbol symbol;
    symbol.type = symbolType::REFERENCE;

    Symbol example(symbolType::NUMBER);
    *example.value = 42;

    *symbol.value = &example;

    std::cout << "ref:\t" << toString(symbol) << std::endl;
  }

  /*
  //any circular reference passed to toString will segfault
  Symbol s, r;
  s.type = symbolType::ARRAY;
  r.type = symbolType::REFERENCE;
  *r.value = &s;
  *s.value = {r};
  std::cout << toString(s) << std::endl;
  */

  //test keywords & operators
  for (int i = 5; i < 50; i++) {
    Symbol symbol;
    symbol.type = symbolType(i);
    std::cout << i << "\t" << toString(symbol) << std::endl;
  }
  return 0;
}

std::string toString(Symbol symbol) {
  switch(symbol.type) {
    //var types
    case symbolType::ARRAY: {
      std::string ret;
      ret = toString(Symbol(symbolType::BRACKET_LEFT));

      auto& array = symbol.value->array;
      for (int i = 0; i < array.size(); i++) {
        ret += toString(array[i]);
        if (i < array.size() -1) {
          ret += toString(Symbol(symbolType::COMMA));
        }
      };

      ret += toString(Symbol(symbolType::BRACKET_RIGHT));
      return ret;
    }

    case symbolType::FUNCTION: {
      std::string ret;
      for (auto& it : symbol.value->function) {
        ret += toString(it);
      }
      return ret;
    }

    case symbolType::NUMBER:
        return std::to_string(symbol.value->number);

    case symbolType::REFERENCE:
        return std::string() +
          toString(symbolType::REFERENCE_MARK) +
          toString(*(symbol.value->reference));

    case symbolType::STRING:
      return symbol.value->string;

    //reserved keywords
    case symbolType::BREAK: return "break";
    case symbolType::CASE: return "case";
    case symbolType::CONTINUE: return "continue";
    case symbolType::DEFAULT: return "default";
    case symbolType::ELSE: return "else";
    case symbolType::FOR: return "for";
    case symbolType::GOTO: return "goto";
    case symbolType::IF: return "if";
    case symbolType::MAIN: return "main";
    case symbolType::MODULE: return "module";
    case symbolType::PRELOAD: return "preload";
    case symbolType::RETURN: return "return";
    case symbolType::SWITCH: return "switch";
    case symbolType::UNDEFINED: return "undefined";
    case symbolType::USE: return "use";
    case symbolType::VAR: return "var";
    case symbolType::WHILE: return "while";

    //operators
    case symbolType::PARENTHESIS_LEFT: return "(";
    case symbolType::PARENTHESIS_RIGHT: return ")";
    case symbolType::ASSIGNMENT: return "=";
    case symbolType::ADDITION: return "+";
    case symbolType::SUBTRACTION: return "-";
    case symbolType::MULTIPLICATION: return "*";
    case symbolType::DIVISION: return "/";
    case symbolType::MODULUS: return "%";
    case symbolType::COMPARITOR: return "==";
    case symbolType::NOT: return "!";
    case symbolType::AND: return "&&";
    case symbolType::OR: return "||";
    case symbolType::LESS_THAN: return "<";
    case symbolType::GREATER_THAN: return ">";
    case symbolType::LESS_THAN_OR_EQUAL: return "<=";
    case symbolType::GREATER_THAN_OR_EQUAL: return ">=";
    case symbolType::BRACE_LEFT: return "{";
    case symbolType::BRACE_RIGHT: return "}";
    case symbolType::BRACKET_LEFT: return "[";
    case symbolType::BRACKET_RIGHT: return "]";
    case symbolType::REFERENCE_MARK: return "&";
    case symbolType::CONCATENATION: return ".";
    case symbolType::SEPARATOR: return ":";
    case symbolType::QUOTE: return "\"";
    case symbolType::COMMA: return ",";

    default:
      return "unknown";
  };
}

Symbol parseString(std::string) {
  //TODO: incomplete
}

int tokenize(std::string str, std::list<Symbol>& target) {
  while(str.size()) {
    //TODO: incomplete
  }

  return 0;
}

inline Symbol popToken(std::string& str) {
  char c;
  switch(c = popFront(str)) {
    //operators
    case '(': return symbolType::PARENTHESIS_LEFT;
    case ')': return symbolType::PARENTHESIS_RIGHT;
    case '=':
      if (str[0] == '=') {
        popFront(str);
        return symbolType::COMPARITOR;
      }
      else {
       return symbolType::ASSIGNMENT;
      }
    case '+': return symbolType::ADDITION;
    case '-': return symbolType::SUBTRACTION;
    case '*': return symbolType::MULTIPLICATION;
    case '/':
      if (str[0] == '/') {
        popFront(str);
        return symbolType::COMMENT;
      }
      else {
       return symbolType::DIVISION;
      }
    case '%': return symbolType::MODULUS;
    case '!': return symbolType::NOT;
    case '&':
      if (str[0] == '&') {
        popFront(str);
        return symbolType::AND;
      }
      else {
       return symbolType::REFERENCE_MARK;
      }
    case '|':
      if (str[0] == '|') {
        popFront(str);
        return symbolType::OR;
      }
      else {
       return symbolType::PIPE;
      }
    case '<':
      if (str[0] == '=') {
        popFront(str);
        return symbolType::LESS_THAN_OR_EQUAL;
      }
      else {
       return symbolType::LESS_THAN;
      }
    case '>':
      if (str[0] == '=') {
        popFront(str);
        return symbolType::GREATER_THAN_OR_EQUAL;
      }
      else {
       return symbolType::GREATER_THAN;
      }
    case '{': return symbolType::BRACE_LEFT;
    case '}': return symbolType::BRACE_RIGHT;
    case '[': return symbolType::BRACKET_LEFT;
    case ']': return symbolType::BRACKET_RIGHT;
    case '.': return symbolType::CONCATENATION;
    case ':': return symbolType::SEPARATOR;
    case '"': return symbolType::QUOTE;
    case ','; return symbolType::COMMA;

    default:
      str = std::string() + c + str;
      if (isalpha(c) || c == '_') {
        std::string word;
        while(isalnum(str[0]) || str[0] == '_') {
          word += popFront(str);
        }

        //find the keyword
        //TODO: incomplete
      }
      if (isdigit(c)) {
        //TODO: incomplete
      }
  }
  return -1;
}

char popFront(std::string& str) {
  char ret = str[0];
  str = str.substr(1, str.size()-1);
  return ret;
}
