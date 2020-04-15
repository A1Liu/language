#include "hash.h"
#include "lexer.h"
#include "parser.h"
#include "util.h"
#include <assert.h>
#include <iostream>

const char *s = R"(
hi = bye(hello + 2, 3 *2 )
print(hi)
)";

int main() {

  uint32_t data_type = 8;
  uint32_t offset = 42;

  Hash h(12, data_type);
  for (uint32_t i = 0; i < 32; i++) {
    *((uint64_t *)h.insert(i, data_type)) = i + offset;
  }

  for (uint32_t i = 0; i < 32; i++) {
    uint64_t *value = ((uint64_t *)h.find(i, data_type));
    assert(value != nullptr);
    assert(*value == i + offset);
  }

  h.remove(1, data_type);
  uint64_t *value = ((uint64_t *)h.find(1, data_type));
  assert(value == nullptr);

  std::cout << "success!" << std::endl;
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
