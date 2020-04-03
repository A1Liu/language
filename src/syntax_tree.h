#pragma once
#include <string_view>
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
    int64_t int_value;
    double float_value;
    std::string_view ident;
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
  Expr(int64_t i) : type(ExprType::Int), int_value(i) {}
  Expr(double i) : type(ExprType::Float), float_value(i) {}
  Expr(std::string_view i) : type(ExprType::Ident), ident(i) {}

  Expr &operator=(ExprType i) {
    type = i;
    return *this;
  }

  Expr &operator=(int64_t i) {
    type = ExprType::Int;
    int_value = i;
    return *this;
  }

  Expr &operator=(double i) {
    type = ExprType::Float;
    float_value = i;
    return *this;
  }

  Expr &operator=(std::string_view i) {
    type = ExprType::Ident;
    ident = i;
    return *this;
  }
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
