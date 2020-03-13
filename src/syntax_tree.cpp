#include "syntax_tree.h"
#include <ostream>

std::ostream &operator<<(std::ostream &os, const NoneExpr &expr) {
  return os << "None";
}

std::ostream &operator<<(std::ostream &os, const Function &func) {
  os << "def " << func.name << "() [ ";
  for (auto stmt : func.statements) {
    os << stmt << ' ';
  }
  os << ']';
  return os;
}

std::ostream &operator<<(std::ostream &os, const Pass &pass) {
  return os << "pass";
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

Statement::Statement(const Function &func) { this->data = func; }

Statement::Statement(const Return &ret) { this->data = ret; }

Statement::Statement() { this->data = Pass(); }

std::ostream &operator<<(std::ostream &os, const Return &ret) {
  os << "Return(" << ret.expr << ')';
  return os;
}

std::ostream &operator<<(std::ostream &os, const Expression &expression) {
  switch (expression.expr.index()) {
  case 0:
    os << std::get<0>(expression.expr);
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
