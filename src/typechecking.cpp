#include "typechecking.h"

// Propogate
#define prop(expr)                                                             \
  if (!(expr)) {                                                               \
    return false;                                                              \
  }

#define assert_warn(expr, str_view, message)                                   \
  if (!(expr)) {                                                               \
    warnings.emplace_back(str_view, message);                                  \
  }

// Propogate and error out
#define prop_err(expr, str_view, message)                                      \
  if (!(expr)) {                                                               \
    error = TypeCheckError{str_view, message};                                 \
    return false;                                                              \
  }

SymbolTable::SymbolTable(BucketArray *buckets)
    : symbols(buckets, 10, sizeof(Type)) {}

TypeChecker::TypeChecker(BucketArray *buckets) {}

bool TypeChecker::check_program(Program *program) {
  for (Stmt &stmt : program->statements) {
    prop(check_statement(&stmt));
  }
  return true;
}

bool TypeChecker::check_statement(Stmt *stmt) {
  switch (stmt->type) {
  case StmtType::Pass:
    return true;
  case StmtType::Expr:
    prop(check_expr(&stmt->expr));
    break;
  case StmtType::Assign:
    break;
  }
  return true;
}

bool TypeChecker::check_expr(Expr *expr) {
  if (expr->inferred_type != InferredType::Unknown) {
    return true;
  }

  switch (expr->type) {
  case ExprType::None:
    expr->inferred_type = InferredType::None;
    break;
  case ExprType::Int:
    expr->inferred_type = InferredType::Int;
    break;
  case ExprType::Float:
    expr->inferred_type = InferredType::Float;
    break;
  }
  return true;
}
