#pragma once
#include <ostream>
#include <string_view>
#include <variant>
#include <vector>

enum class ExprType { None, Int, Float, Ident };
struct Expr {
  ExprType type;
  union {
    int64_t int_value;
    double float_value;
    std::string_view ident;
  };

  Expr(ExprType _type) : type(_type) {}
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

enum class StmtType { Pass, Expr };
struct Stmt {
  StmtType type;
  union {
    Expr expr;
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
