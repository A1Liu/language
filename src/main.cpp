#include "lexer.h"
#include "parser.h"
#include <iostream>

int main() {
  std::string s = R"(

def hi():
    return 1 + 12.12
    def hi():
      pass

  )";

  Lexer l{s};
  Program program;

  Parser p{l};

  bool a = p.try_parse_program(program);

  if (!a)
    std::cout << "failed" << std::endl;
  std::cout << program << std::endl;
}
