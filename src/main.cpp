#include "ast_reader.hpp"
#include "lexer.hpp"
#include "parser.hpp"

#include <fstream>
#include <iostream>

void runPrompt() {
  char buffer[1024];
  ASTReaderPrefix reader;

  for (;;) {
    std::cout << ">";
    std::cin.getline(buffer, 1024);
    Lexer lexer(buffer);
    Parser parser(lexer.GetTokenVector());
    std::vector<Stmt*> statements = parser.GetStmtVector();

    for (Stmt* stmt : statements) {
      reader.Print(stmt);
      std::cout << std::endl;
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc == 1) {
    runPrompt();
  }

  for (int i = 1; i < argc; i++) {
    std::ifstream is(argv[i], std::fstream::in);
    std::string source;

    getline(is, source, '\0');

    is.close();

    Lexer lexer(source);
    Parser parser(lexer.GetTokenVector());
    std::vector<Stmt*> statements = parser.GetStmtVector();

    std::cout << "statement count: " << statements.size() << std::endl;
  }

  return 0; 
}
