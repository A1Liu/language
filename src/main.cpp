#include "lexer.h"
#include "parser.h"
#include "pools.h"
#include <iostream>

int main() {
  std::string s = R"(
hi = 12
)";

  BucketArray buckets;
  Lexer l{s};
  Parser p(&l);
  Program program;

  bool a = p.try_parse_program(program);
  for (Token &tok : p.tokens) {
    std::cout << "(" << tok << ") ";
  }
  std::cout << std::endl;

  if (!a) {
    std::cout << "failed with errors:" << std::endl;
    for (auto err : p.errors) {
      std::cout << "  at '" << err.location << "' had error: " << err.message
                << std::endl;
    }
  }
  std::cout << program << std::endl;
}
