#pragma once
#include "lexer.h"
#include "pools.h"
#include "syntax_tree.h"

struct Parser {
  struct ParseError {
    std::string_view location;
    const char *message;
  };

  std::vector<Token> tokens;
  std::vector<ParseError> errors;
  int index = 0;

  Parser(Lexer *lexer);

  const Token &peek();
  const Token &pop();

  bool try_parse_program(Program &program);
  bool try_parse_statement(Stmt &statement);

  bool try_parse_add(Expr &expr);
  bool try_parse_mul(Expr &expr);

  bool try_parse_unary_prefix(Expr &expr);
  bool try_parse_unary_postfix(Expr &expr);

  bool try_parse_expr(Expr &expr);
  bool try_parse_atom_expr(Expr &expr);

    bool try_parse_comma_list(std::vector<Expr> &expressions);
};
