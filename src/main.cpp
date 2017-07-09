#include "lexer.hpp"
#include "parser.hpp"

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

    Parser parser(lexer.GetTokenList());

    std::list<Stmt*> statements = parser.GetStatements();

    std::cout << "statement count: " << statements.size() << std::endl;
  }

  return 0; 
}
