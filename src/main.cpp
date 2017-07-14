#include "ast_deleter.hpp"
#include "ast_duplicator.hpp"
#include "ast_reader.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "interpreter.hpp"

#include <fstream>
#include <iostream>

ASTDeleter deleter;
ASTReaderPrefix reader;
Interpreter interpreter;

void run(std::string source) {
  Lexer lexer(source);
  Parser parser(lexer.GetTokenVector());
  std::vector<Stmt*> statements = parser.GetStmtVector();

  for (Stmt* stmt : statements) {
//    ASTDuplicator duplicator;

    interpreter.Execute(stmt);

//    reader.Print(duplicator.DuplicateAST(stmt));
//    std::cout << std::endl;
    deleter.DeleteAST(stmt);

    //handle returns
    if (interpreter.GetReturnCalled()) {
      std::cout << "Final value: " << interpreter.GetResult().ToString() << std::endl;
      break;
    }
  }
}

void runPrompt() {
  char buffer[1024];

  for (;;) {
    std::cout << ">";
    std::cin.getline(buffer, 1024);
    run(buffer);
  }
}

int main(int argc, char* argv[]) {
  if (argc == 1) {
    runPrompt();
  }

  for (int i = 1; i < argc; i++) {
    std::string source;

    std::ifstream is(argv[i], std::fstream::in);
    getline(is, source, '\0');
    is.close();

    run(source);
  }

  return 0; 
}
