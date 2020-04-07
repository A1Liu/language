#pragma once
#include "util.h"
#include <vector>

enum class ExprType {
  None,
  Int,
  Float,
  Ident,
  Tup,
  Call,
  Add,
  Sub,
  Mul,
  Div,
  Mod,
  Leq,
  Geq,
  Lt,
  Gt,
  Eq,
  Neq,
  Deref,
  Range,
  RangePrefix,
  RangePostfix
};

struct Expr {
  ExprType type;
  union {
    uint64_t int_value;
    double float_value;
    String ident;
    struct {
      Expr *callee;
      Expr *call_tup_begin, *call_tup_end;
    };
    struct {
      Expr *tup_begin, *tup_end;
    };
    struct {
      Expr *left, *right;
    };
    struct {
      Expr *operand;
    };
  };

  Expr() : type(ExprType::None) {}
  Expr(ExprType _type) : type(_type) {}
  Expr(ExprType _type, Expr *a, Expr *b) : type(_type), left(a), right(b) {}
  Expr(uint64_t i) : type(ExprType::Int), int_value(i) {}
  Expr(double i) : type(ExprType::Float), float_value(i) {}
};

enum class TypeType { Int, Float, Func };
struct Type {
  TypeType type;
  union {
    struct {
      Type *return_type;
      Type *param_types_begin, *param_types_end;
    };
  };
};

enum class StmtType { Pass, Expr, Assign };
struct Stmt {
  StmtType type;
  union {
    Expr expr;
    struct {
      Expr assign_target, assign_value;
    };
  };

  Stmt() : type(StmtType::Pass) {}
  Stmt(Expr &_expr) : type(StmtType::Expr), expr(_expr) {}

  Stmt &operator=(StmtType i) {
    type = i;
    return *this;
  }
};

struct Program {
  std::vector<Stmt> statements;
};

std::ostream &operator<<(std::ostream &os, const Stmt &stmt);
std::ostream &operator<<(std::ostream &os, const Expr &expr);
std::ostream &operator<<(std::ostream &os, const Program &program);
