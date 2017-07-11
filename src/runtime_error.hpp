#pragma once

#include <string>

class RuntimeError {
public:
  RuntimeError() = delete;
  RuntimeError(int l, std::string msg): line(l), errmsg(msg) {}
  int GetLine() { return line; }
  std::string GetErrMsg() { return errmsg; }
private:
  const int line;
  std::string errmsg;
};
