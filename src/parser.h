#pragma once
#include "lexer.h"
#include "pools.h"
#include "syntax_tree.h"

struct Parser {
  std::vector<Token> tokens;
  BucketArray *buckets;
  uint64_t size = 0;

  int index = 0;

  Parser(BucketArray *buckets, Lexer *lexer);

  const Token &peek();
  Token &pop();

  bool try_parse_program(Program &program);
  bool try_parse_statement(Stmt &statement);
  bool try_parse_atom_expr(Expr &expr);
};
