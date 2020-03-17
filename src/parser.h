#pragma once
#include "lexer.h"
#include "syntax_tree.h"
#include <optional>

struct Parser {

  std::string_view view;
  std::vector<Token> tokens;
  std::vector<TokenType> binary_op_stack;
  int index = 0;

  Parser(Lexer &lexer);

  const Token &peek();
  Token &pop();
  int idx();
  void reset(int idx);

  bool try_parse_program(Program &program);
  bool try_parse_statement(Statement &statement);
  bool try_parse_function(Function &func);
  bool try_parse_return(Return &ret);
  bool try_parse_expr(Expression &expr);
  bool try_parse_unary_expr(Expression &expr);
  bool try_parse_postfix_expr(Expression &expr);
  bool try_parse_atom_expr(Expression &expr);
};
