#include "lexer.h"
#include "parser.h"
#include <iostream>

int main() {
  std::string s = R"(

def hi():
    return None

)";

  Lexer l{s};
  Program program;

  Parser p{l};

  p.try_parse_program(program);

  std::cout << program << std::endl;

  // while (l.has_next()) {
  //   std::cout << l.next() << std::endl;
  // }
}
