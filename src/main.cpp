#include "hash.h"
#include "lexer.h"
#include "parser.h"
#include "util.h"
#include <iostream>

const char *s = R"(
hi = bye(hello + 2, 3 *2 )
print(hi)
)";

int main() {

  Hash h = hashCreate(12, 64);
  *((uint64_t *)hashInsert(&h, 12)) = 12;
  uint64_t *value = ((uint64_t *)hashFind(&h, 12));

  std::cout << value << std::endl;
  std::cout << *value << std::endl;
  std::exit(0);

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
