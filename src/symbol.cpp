#include "symbol.hpp"

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
