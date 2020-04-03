#include "lexer.h"
#include "parser.h"
#include "pools.h"
#include <iostream>

int main() {
  std::string s = R"(
hi = (hello, hi)
)";

  BucketArray buckets;
  Lexer l{s};
  Parser p(&buckets, &l);
  Program program;

  bool a = p.try_parse_program(program);
  for (Token &tok : p.tokens) {
    std::cout << "(" << tok << ") ";
  }
  std::cout << std::endl;

  if (!a) {
    std::cout << "error" << std::endl;
    std::cout << "at '" << p.error.location
              << "' had error: " << p.error.message << std::endl;
  }

  std::cout << program << std::endl;
  buckets.free();
  p.pool.free();
}
