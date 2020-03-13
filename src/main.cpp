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

  bool a = p.try_parse_program(program);

  for (Token& tok : p.tokens) {
      std::cout << tok << std::endl;
  }

  if (!a) std::cout << "failed" << std::endl;
  std::cout << program << std::endl;

  // while (l.has_next()) {
  //   std::cout << l.next() << std::endl;
  // }
}
