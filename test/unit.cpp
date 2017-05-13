#include "unit.hpp"

#include <iostream>

//unit test utilities
int unitCount = 0;
int failCount = 0;

func_t tests[] = {
  toString_test,
  nullptr
};

void unit(std::string name, std::function<bool()> f) {
  std::cout << name << "\t\t" <<
  (f() ? (unitCount++, "OK") : (failCount++, "fail"))
  << std::endl;
}

int main(int argc, char* argv[]) {
  for(int i = 0; tests[i]; i++) {
    tests[i](argc, argv);
  }

  std::cout << "Total fails: " << failCount << std::endl;

  return 0;
}
