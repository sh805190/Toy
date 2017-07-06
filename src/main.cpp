#include "token.hpp"

#include <iostream>

int main() {
  Token t(TokenType::BANG, "!", 0, 1);
  std::cout << t.ToString() << std::endl;
  return 0; 
}
