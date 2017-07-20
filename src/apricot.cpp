#include "error_handler.hpp"
#include "lexer.hpp"
#include "token.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

void run(std::string source) {
{
  Lexer lexer(source);

  std::vector<Token> tokenVector = lexer.GetTokenVector();

  for (Token& tok : tokenVector) {
    std::cout << tok.ToString() << std::endl;
  }

  if (ErrorHandler::GetErrorCount()) {
    std::cout << "Total errors found: " << ErrorHandler::GetErrorCount() << std::endl;
    ErrorHandler::SetErrorCount(0);
  }
}
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
