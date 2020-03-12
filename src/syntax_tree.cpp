#include "syntax_tree.h"
#include <ostream>

std::ostream &operator<<(std::ostream &os, const Function &function) {
  os << "def " << function.name << "()";
  return os;
}

std::ostream &operator<<(std::ostream &os, const Statement &statement) {
  os << "Statement(";
  switch (statement.type) {
  case Statement::StmtType::Function:
    os << statement.func;
    break;
  case Statement::StmtType::Return:
    os << statement.retStmt;
    break;
  }
  os << ')';
  return os;
}

Statement::Statement(const Function &func) {
  type = StmtType::Function;
  this->func = func;
}

Statement::Statement(const Return &ret) {
  type = StmtType::Return;
  this->retStmt = ret;
}

Statement::Statement(const Statement &other) { assign_to_left(*this, other); }

Statement::~Statement() {
  if (type == StmtType::Function) {
    // func.~Function();
  }
}

Statement &Statement::assign_to_left(Statement &left, const Statement &right) {
  left.type = right.type;
  switch (right.type) {
  case StmtType::Return:
    left.retStmt = right.retStmt;
    break;
  case StmtType::Function:
    left.func = right.func;
    break;
  }
  return left;
}

Statement &Statement::operator=(const Statement &other) {
  return assign_to_left(*this, other);
}

std::ostream &operator<<(std::ostream &os, const Return &aReturn) {
  os << "Return(" << aReturn.expr << ')';
  return os;
}

std::ostream &operator<<(std::ostream &os, const Expression &expression) {
  switch (expression.type) {
  case Expression::ExprType::None:
    os << "None";
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
