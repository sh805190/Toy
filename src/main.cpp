#include "lexer.hpp"

#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " fname [fname...]" << std::endl;
    return 0;
  }

  for (int i = 1; i < argc; i++) {
    std::ifstream is(argv[i], std::fstream::in);
    std::string source;

    getline(is, source, '\0');

    is.close();

    Lexer lexer(source);

    std::list<Token> tokens = lexer.GetTokenList();

    std::cout << "size: " << tokens.size() << std::endl;

    for (Token token : tokens) {
      std::cout << token.ToString() << std::endl;
    }
  }

  return 0; 
}
