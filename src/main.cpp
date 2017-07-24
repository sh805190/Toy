#include "ast_deleter.hpp"
#include "ast_duplicator.hpp"
#include "ast_reader.hpp"
#include "error_handler.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "interpreter.hpp"

#include <fstream>
#include <iostream>

static Interpreter* interpreter = nullptr;

static ASTDeleter deleter;
static ASTReader reader;

void run(std::string source) {
  Lexer lexer(source);
  Parser parser(lexer.GetTokenVector());
  std::vector<Stmt*> statements = parser.GetStmtVector();

  for (Stmt* stmt : statements) {

    //debugging
    std::cout << "READER:";
    reader.Print(stmt);
    std::cout << std::endl;

    interpreter->Execute(stmt);
    deleter.DeleteAST(stmt);

    //handle returns
    if (interpreter->GetReturnCalled()) {
      std::cout << "Final value: " << interpreter->GetResult()->ToString() << std::endl;
      break;
    }
  }
}

void runPrompt() {
  char buffer[1024];

  while (!interpreter->GetReturnCalled()) {
    std::cout << ">";
    std::cin.getline(buffer, 1024);
    run(buffer);
  }
}

int main(int argc, char* argv[]) {
  //create within this "scope"
  interpreter = new Interpreter();

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

  //delete within this "scope"
  delete interpreter;

  //error checking
  if (Literal::count || Stmt::count || Expr::count) {
    ErrorHandler::Error(-1, "Memory leak detected");
    std::cout << "Literals Collected: " << GarbageCollector<Literal>::Collect() << std::endl;
    std::cout << "Stmt Collected: " << GarbageCollector<Stmt>::Collect() << std::endl;
    std::cout << "Expr Collected: " << GarbageCollector<Expr>::Collect() << std::endl;
  }

  if (ErrorHandler::GetErrorCount()) {
    std::cout << "Total errors found: " << ErrorHandler::GetErrorCount() << std::endl;
    ErrorHandler::SetErrorCount(0);
  }

  return 0; 
}
