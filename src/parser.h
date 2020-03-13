#pragma once
#include "lexer.h"
#include "syntax_tree.h"
#include <optional>

struct Parser {

  std::string_view view;
  std::vector<Token> tokens;
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
  bool try_parse_expression(Expression &expr);
};
