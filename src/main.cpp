#include "lexer.h"
#include "parser.h"
#include "pools.h"
#include <iostream>

int main() {
  std::string s = R"(
hi = bye(hello + 2, 3 *2 )
print(hi)
)";

  BucketArray buckets;
  Parser p(&buckets, s);
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
}
