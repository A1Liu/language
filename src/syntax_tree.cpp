#include "syntax_tree.h"
#include <ostream>

std::ostream &operator<<(std::ostream &os, const NoneExpr &expr) {
  return os << "None";
}

std::ostream &operator<<(std::ostream &os, const IntegerLiteral &lit) {
  return os << lit.value;
}
std::ostream &operator<<(std::ostream &os, const FloatLiteral &lit) {
  return os << lit.value;
}

std::ostream &operator<<(std::ostream &os, const UnaryOp &uop) {
  return os << uop.type << *uop.operand;
}
std::ostream &operator<<(std::ostream &os, const BinaryOp &bop) {
  return os << *bop.left << ' ' << bop.type << ' ' << *bop.left;
}
std::ostream &operator<<(std::ostream &os, const Identifier &ident) {
  return os << ident.name;
}

std::ostream &operator<<(std::ostream &os, const Pass &pass) {
  return os << "pass";
}

std::ostream &operator<<(std::ostream &os, const Function &func) {
  os << "def " << func.name << "() [ ";
  for (auto stmt : func.statements) {
    os << stmt << ' ';
  }
  os << ']';
  return os;
}

std::ostream &operator<<(std::ostream &os, const BinaryOpType &boptype) {
  switch (boptype) {
  case BinaryOpType::Plus:
    return os << '+';
  case BinaryOpType::Multiply:
    return os << '*';
  case BinaryOpType::Divide:
    return os << '/';
  case BinaryOpType::Minus:
    return os << '-';
  case BinaryOpType::DoubleDivide:
    return os << "//";
  case BinaryOpType::Modulo:
    return os << '%';
  }
}

std::ostream &operator<<(std::ostream &os, const UnaryOpType &uoptype) {
  switch (uoptype) {
  case UnaryOpType::Splat:
    return os << '*';
  case UnaryOpType::DoubleSplat:
    return os << "**";
  case UnaryOpType::Negate:
    return os << '-';
  }
}

std::ostream &operator<<(std::ostream &os, const Return &ret) {
  os << "Return(" << ret.expr << ')';
  return os;
}

std::ostream &operator<<(std::ostream &os, const Statement &statement) {
  os << "Statement(";
  switch (statement.data.index()) {
  case 0:
    os << std::get<0>(statement.data);
    break;
  case 1:
    os << std::get<1>(statement.data);
    break;
  case 2:
    os << std::get<2>(statement.data);
    break;
  }
  os << ')';
  return os;
}

std::ostream &operator<<(std::ostream &os, const Expression &expression) {
  switch (expression.expr.index()) {
  case 0:
    return os << std::get<0>(expression.expr);
  case 1:
    return os << std::get<1>(expression.expr);
  case 2:
    return os << std::get<2>(expression.expr);
  case 3:
    return os << std::get<3>(expression.expr);
  case 4:
    return os << std::get<4>(expression.expr);
  case 5:
    return os << std::get<5>(expression.expr);
    // case 1:
    //   os << std::get<1>(expression.expr);
    // case 2:
    //   os << std::get<2>(expression.expr);
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, const Program &program) {
  os << '[' << ' ';
  for (auto s : program.statements)
    os << s << ' ';
  os << ']';
  return os;
}
