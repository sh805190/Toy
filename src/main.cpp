#include "symbol.hpp"

#include <functional>
#include <iostream>

//unit test utilities
static int unitCount = 0;
static int failCount = 0;
void unit(std::string name, std::function<bool()> f) {
  std::cout << name << "\t\t" <<
  (f() ? (unitCount++, "OK") : (failCount++, "fail"))
  << std::endl;
}

//forward declarations
std::string toString(Symbol symbol);

//start the test
int main(int argc, char* argv[]) {

  //test number
  unit("number", []() -> bool {
    Symbol symbol(symbolType::NUMBER);

    *symbol.value = 42;
    bool whole = !toString(symbol).compare("42");

    *symbol.value = 42.3;
    bool decimal = !toString(symbol).compare("42.3");

    return whole && decimal;
  });

  //test string
  unit("string", []() -> bool {
    Symbol symbol(symbolType::STRING);
    *symbol.value = "foobar";
    return !toString(symbol).compare("foobar");
  });

  //test reference
  unit("reference", []() -> bool {
    Symbol symbol;
    symbol.type = symbolType::REFERENCE;

    Symbol example(symbolType::NUMBER);
    *example.value = 42;

    *symbol.value = &example;

    return !toString(symbol).compare("&42");
  });

  //test array
  unit("array", []() -> bool {
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
    *member3.value = 3.33;
    *member4.value = 4;
    *member5.value = 5;

    *symbol.value = {
      member1,
      member2,
      member3,
      member4,
      member5
    };

    return !toString(symbol).compare("[1,2,3.33,4,5]");
  });

  //TODO: test function

  //test keywords & operators
  for (int i = 5; i < symbolType::SYMBOL_LAST; i++) {
    unit("", [i]() -> bool {
      Symbol symbol;
      symbol.type = symbolType(i);

      std::cout << toString(symbol);

      return !toString(symbol)
        .compare(symbolConst.at(symbolType(i)));
    });
  }

  std::cout << "Total fails: " << failCount << std::endl;

  return 0;
}

std::string toString(Symbol symbol) {
  //any circular reference passed to toString will segfault
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

    case symbolType::NUMBER: {
      //get the string
      std::string str = std::to_string(
        symbol.value->number
      );

      //erase trailing zeroes
      str.erase(
        str.find_last_not_of('0') +1,
        std::string::npos
      );

      //handle dangling period
      if (str.back() == '.') {
        str.pop_back();
      }

      return str;
    }

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
