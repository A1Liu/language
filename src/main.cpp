#include "lexer.h"
#include <iostream>

int main() {
  std::string s = R"(

   

def hi():
  def hi():
    return None

  

  def hi():
    return None
  return None

    

)";

  Lexer l{s};

  while (l.has_next()) {
    std::cout << l.next() << std::endl;
  }
}
