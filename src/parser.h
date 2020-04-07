#pragma once
#include "lexer.h"
#include "util.h"
#include "syntax_tree.h"
#include <vector>

struct Parser {
  struct ParseError {
    String location;
    const char *message;
  };

  BucketArray *buckets;
  Lexer lexer;
  ParseError error;
  std::vector<Token> tokens;
  int index = 0;

  Parser(BucketArray *buckets, const char *view);

  const Token &peek();
  const Token &pop();

  bool try_parse_program(Program &program);
  bool try_parse_statement(Stmt &statement);

  bool try_parse_compare(Expr &expr);
  bool try_parse_add(Expr &expr);
  bool try_parse_mul(Expr &expr);

  bool try_parse_unary_prefix(Expr &expr);
  bool try_parse_unary_postfix(Expr &expr);

  bool try_parse_expr(Expr &expr);
  bool try_parse_atom_expr(Expr &expr);
};
