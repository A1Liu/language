#pragma once
#include <ostream>
#include <string_view>
#include <variant>
#include <vector>

struct Expression;
struct Function;
struct Return;
struct Statement;

struct NoneExpr {};

struct IntegerLiteral {
  int64_t value;

  explicit IntegerLiteral(int64_t value_) : value(value_) {}
};

struct FloatLiteral {
  double value;
  explicit FloatLiteral(double value_) : value(value_) {}
};

struct Identifier {
  std::string_view name;
  Identifier(std::string_view name_) : name(name_) {}
};

enum class BinaryOpType { Plus, Multiply, Divide, Minus, DoubleDivide, Modulo };
enum class UnaryOpType { Splat, DoubleSplat, Negate, Index };

struct BinaryOp {
  BinaryOpType type;
  Expression *left;
  Expression *right;

  BinaryOp(BinaryOpType type_, Expression *left_, Expression *right_)
      : type(type_), left(left_), right(right_) {}
};

struct UnaryOp {
  UnaryOpType type;
  Expression *operand;

  UnaryOp(UnaryOpType type_, Expression *operand_)
      : type(type_), operand(operand_) {}
};

struct Expression {
  std::variant<NoneExpr, IntegerLiteral, FloatLiteral, Identifier, BinaryOp,
               UnaryOp>
      expr = NoneExpr();
};

struct Return {
  Expression expr;
};
struct Pass {};

struct Function {
  std::string_view name;
  std::vector<Statement> statements;
};

struct Statement {
  std::variant<Function, Return, Pass, Expression> data;

  Statement(const Function &func) { this->data = func; }
  Statement(const Return &ret) { this->data = ret; }
  Statement() { this->data = Pass(); }
};

struct Program {
  std::vector<Statement> statements;
};

std::ostream &operator<<(std::ostream &os, const NoneExpr &);
std::ostream &operator<<(std::ostream &os, const IntegerLiteral &);
std::ostream &operator<<(std::ostream &os, const FloatLiteral &);
std::ostream &operator<<(std::ostream &os, const UnaryOp &);
std::ostream &operator<<(std::ostream &os, const BinaryOp &);
std::ostream &operator<<(std::ostream &os, const Identifier &);
std::ostream &operator<<(std::ostream &os, const BinaryOpType &expression);
std::ostream &operator<<(std::ostream &os, const UnaryOpType &expression);
std::ostream &operator<<(std::ostream &os, const Expression &expression);

std::ostream &operator<<(std::ostream &os, const Return &aReturn);
std::ostream &operator<<(std::ostream &os, const Function &func);
std::ostream &operator<<(std::ostream &os, const Pass &pass);
std::ostream &operator<<(std::ostream &os, const Statement &statement);
std::ostream &operator<<(std::ostream &os, const Program &program);
