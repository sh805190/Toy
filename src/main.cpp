#include "symbol.hpp"

#include <iostream>

std::string toString(Symbol);
//Symbol parseString(std::string);

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

    //keywords & operators
    default:
      if (symbol.type < symbolType::SYMBOL_LAST) {
        return symbolConst.at(symbol.type);
      }
      else {
        //unknown symbol
        return "unknown";
      }
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

char popFront(std::string& str) {
  char ret = str[0];
  str = str.substr(1, str.size()-1);
  return ret;
}
