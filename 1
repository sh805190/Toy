#include "ast_deleter.hpp"
#include "ast_duplicator.hpp"
#include "ast_reader.hpp"
#include "error_handler.hpp"
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

//reader.Print(stmt);
//std::cout << std::endl;

    interpreter.Execute(stmt);
    deleter.DeleteAST(stmt);

    //handle returns
    if (interpreter.GetReturnCalled()) {
      std::cout << "Final value: " << interpreter.GetResult().ToString() << std::endl;
      break;
    }
  }

  if (ErrorHandler::GetErrorCount()) {
    std::cout << "Total errors found: " << ErrorHandler::GetErrorCount() << std::endl;
    ErrorHandler::SetErrorCount(0);
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
