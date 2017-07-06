#pragma once

#include <string>

class ErrorHandler {
public:
  ErrorHandler() = delete;

  static void Error(int line, std::string msg);

  static int SetErrorCount(int i);
  static int GetErrorCount();

private:
  static int errorCount;
};
