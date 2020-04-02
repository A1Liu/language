#include "syntax_tree.h"
#include <ostream>

std::ostream &operator<<(std::ostream &os, const Stmt &stmt) {
  switch (stmt.type) {
  case StmtType::Pass:
    return os << "pass";
  case StmtType::Expr:
    return os << stmt.expr;
  case StmtType::Assign:
    return os << "Assign( " << stmt.assign_target << " = " << stmt.assign_value
              << " )";
  }

  return os;
}

std::ostream &operator<<(std::ostream &os, const Expr &expr) {
  switch (expr.type) {
  case ExprType::None:
    return os << "None";
  case ExprType::Int:
    return os << "i(" << expr.int_value << ")";
  case ExprType::Float:
    return os << "f(" << expr.float_value << ")";
  case ExprType::Ident:
    return os << "id(" << expr.ident << ")";
  case ExprType::Add:
    return os << "Add( " << *expr.left << ", " << *expr.right << " )";
  case ExprType::Sub:
    return os << "Sub( " << *expr.left << ", " << *expr.right << " )";
  case ExprType::Mul:
    return os << "Mul( " << *expr.left << ", " << *expr.right << " )";
  case ExprType::Div:
    return os << "Div( " << *expr.left << ", " << *expr.right << " )";
  case ExprType::Mod:
    return os << "Mod( " << *expr.left << ", " << *expr.right << " )";
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, const Program &program) {
  os << "[ ";
  for (const Stmt &s : program.statements) {
    os << s << ", ";
  }
  return os << " ]";
}
