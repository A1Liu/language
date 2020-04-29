#include "lexer.h"
#include "parser.h"
#include "util.h"
#include <assert.h>
#include <iostream>

const char *s = R"(
hi = bye(hello + 2, 3 *2, (32, (3, 2,21) , 1), 3*2, 3*2, 3*2, 3*2, 3*2, 3*2, 3*2, 3*2, 3*2, 3*2, 3*2, 3*2, 3*2 )
print(hi)
)";

int main() {
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
