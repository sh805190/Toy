#include "../src/symbol.hpp"

#include "unit.hpp"

//start the test
int toString_test(int argc, char* argv[]) {

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
    Symbol symbol;
    symbol.type = symbolType(i);

    unit(toString(symbol), [symbol,i]() -> bool {
      return !toString(symbol)
        .compare(symbolConst.at(symbolType(i)));
    });
  }

  return 0;
}
