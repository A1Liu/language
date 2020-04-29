#pragma once

#include "syntax_tree.h"
#include "util.h"
#include <vector>

struct TypeChecker {
  struct TypeCheckError {
    String location;
    const char *message;
  };

  TypeCheckError error;
  std::vector<TypeCheckError> warnings;
  Hash symbol_table;

  bool check_program(Program *program);
  bool check_statement(Stmt *statement);
  bool check_expr(Expr *expr);
  bool check_arithmetic(Expr *expr);
  bool check_comparison(Expr *expr);
};
