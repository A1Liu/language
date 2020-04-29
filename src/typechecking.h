#pragma once

#include "syntax_tree.h"
#include "util.h"
#include <vector>

struct SymbolTable {
  Hash symbols;
  SymbolTable *parent;

  SymbolTable(BucketArray *buckets);
};

struct TypeChecker {
  struct TypeCheckError {
    String location;
    const char *message;
  };

  TypeChecker(BucketArray *buckets);

  TypeCheckError error;
  std::vector<TypeCheckError> warnings;
  SymbolTable *sym = nullptr;

  bool check_program(Program *program);
  bool check_statement(Stmt *statement);
  bool check_expr(Expr *expr);
  bool check_arithmetic(Expr *expr);
  bool check_comparison(Expr *expr);
};
