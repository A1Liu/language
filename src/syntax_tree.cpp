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
  case ExprType::Call: {
    os << "call<" << *expr.callee << ">( ";
    int i = 0;
    for (Expr *e = expr.call_tup_begin; e != expr.call_tup_end; e++) {
      os << *e << ",";
    }
    return os << " )";
  }
  case ExprType::Tup: {
    os << "tup( ";
    int i = 0;
    for (Expr *e = expr.tup_begin; e != expr.tup_end; e++) {
      os << *e << ",";
    }
    return os << " )";
  }
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
  case ExprType::Eq:
    return os << "Eq( " << *expr.left << ", " << *expr.right << " )";
  case ExprType::Neq:
    return os << "Neq( " << *expr.left << ", " << *expr.right << " )";
  case ExprType::Leq:
    return os << "Leq( " << *expr.left << ", " << *expr.right << " )";
  case ExprType::Geq:
    return os << "Geq( " << *expr.left << ", " << *expr.right << " )";
  case ExprType::Gt:
    return os << "Gt( " << *expr.left << ", " << *expr.right << " )";
  case ExprType::Lt:
    return os << "Lt( " << *expr.left << ", " << *expr.right << " )";
  case ExprType::Deref:
    return os << "Deref( " << *expr.operand << " )";
  case ExprType::Range:
    return os << "Range( " << *expr.operand << " )";
  case ExprType::RangePrefix:
    return os << "RangePrefix( " << *expr.operand << " )";
  case ExprType::RangePostfix:
    return os << "RangePostfix( " << *expr.operand << " )";
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
