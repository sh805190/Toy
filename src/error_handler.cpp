#include "error_handler.hpp"

#include <iostream>

int ErrorHandler::errorCount = 0;

void ErrorHandler::Error(int line, std::string msg) {
  std::cerr << "[line " << line << "] Error: " << msg << std::endl;
  errorCount++;
}

int ErrorHandler::SetErrorCount(int i) {
  return errorCount = i;
}

int ErrorHandler::GetErrorCount() {
  return errorCount;
}
