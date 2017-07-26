#include "run.hpp"

#include "ast_deleter.hpp"
#include "ast_reader.hpp"
#include "error_handler.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "stmt.hpp"

#include <vector>

//TMP
#include <iostream>

void run(std::string source, Interpreter* interpreter, bool moduleFlag) {
  ASTDeleter deleter;
  ASTReader reader;

  Lexer lexer(source);
  Parser parser(lexer.GetTokenVector(), moduleFlag);
  std::vector<Stmt*> statements = parser.GetStmtVector();

  for (Stmt* stmt : statements) {
    //if panic state
    if (ErrorHandler::GetErrorCount() && interpreter->GetFlag("panic")) {
      break;
    }

    //debugging
    std::cout << "READER:";
    reader.Print(stmt);
    std::cout << std::endl;

    interpreter->Execute(stmt);
    deleter.DeleteAST(stmt);

    //debugging
    std::cout << "INTERPRETER:" << interpreter->GetResult()->ToString() << std::endl;

    //handle returns
    if (interpreter->GetReturnCalled()) {
      std::cout << "Final value: " << interpreter->GetResult()->ToString() << std::endl;
      break;
    }
  }
}
