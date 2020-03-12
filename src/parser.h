#pragma once
#include "lexer.h"
#include "syntax_tree.h"

struct Parser {

  std::string_view view;
  std::vector<Token> tokens;
  int index = 0;

  Parser(Lexer &lexer);

  Program parse();
  bool try_parse_program(Program &program);
  bool try_parse_function(Function &func);
  bool try_parse_return(Return &ret);
};
