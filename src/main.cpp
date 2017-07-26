#include "error_handler.hpp"
#include "garbage_collector.hpp"
#include "interpreter.hpp"
#include "run.hpp"

#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {
  //create within this "scope"
  Interpreter* interpreter = new Interpreter();

  if (argc == 1) {
    char buffer[1024];

    while (!interpreter->GetReturnCalled() && !(ErrorHandler::GetErrorCount() && interpreter->GetFlag("panic")) ) {
      std::cout << ">";
      std::cin.getline(buffer, 1024);
      run(buffer, interpreter);
    }
  }
  else {
    for (int i = 1; i < argc; i++) {
      std::string source;

      std::ifstream is(argv[i], std::fstream::in);
      getline(is, source, '\0');
      is.close();

      run(source, interpreter);
    }
  }

  //check for panic state
  if (ErrorHandler::GetErrorCount() && interpreter->GetFlag("panic")) {
    std::cout << "Panic state detected" << std::endl;
  }

  //delete within this "scope"
  delete interpreter;

  //error checking
  if (Literal::count || Stmt::count || Expr::count) {
    ErrorHandler::Error(-1, "Memory leak detected");
    //order of precedence
    std::cout << "Stmt Collected: " << GarbageCollector<Stmt>::Collect() << std::endl;
    std::cout << "Expr Collected: " << GarbageCollector<Expr>::Collect() << std::endl;
    std::cout << "Literals Collected: " << GarbageCollector<Literal>::Collect() << std::endl;
  }

  if (ErrorHandler::GetErrorCount()) {
    std::cout << "Total errors found: " << ErrorHandler::GetErrorCount() << std::endl;
    ErrorHandler::SetErrorCount(0);
  }

  return 0; 
}
