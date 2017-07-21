#include "ast_deleter.hpp"
#include "ast_reader.hpp"
#include "error_handler.hpp"
#include "garbage_collector.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "stmt.hpp"
#include "token.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

void run(std::string source) {
{
  ASTDeleter deleter;
  ASTReaderPrefix reader;

  Lexer lexer(source);
  Parser parser(lexer.GetTokenVector());
  std::vector<Stmt*> stmtVector = parser.GetStmtVector();

  for (Stmt* stmt : stmtVector) {
    reader.Print(stmt);
    std::cout << std::endl;
    deleter.DeleteAST(stmt);
  }

  if (ErrorHandler::GetErrorCount()) {
    std::cout << "Total errors found: " << ErrorHandler::GetErrorCount() << std::endl;
    ErrorHandler::SetErrorCount(0);
  }
}
std::cout << "Literal::count = " << Literal::count << std::endl;
std::cout << "Collected: " << GarbageCollector<Literal>::Collect() << std::endl;
std::cout << "Collected: " << GarbageCollector<Literal>::GetSize() << std::endl;
std::cout << "Literal::count = " << Literal::count << std::endl;
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
