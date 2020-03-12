#include <ostream>
#include <string_view>
#include <vector>

struct Expression;
struct Function;
struct Return;
struct Statement;

struct Expression {
  enum class ExprType { None };
  ExprType type;

  friend std::ostream &operator<<(std::ostream &os,
                                  const Expression &expression);
};

struct Return {
  Expression expr;
  friend std::ostream &operator<<(std::ostream &os, const Return &aReturn);
};

struct Function {
  std::string_view name;
  std::vector<Statement> statements;

  friend std::ostream &operator<<(std::ostream &os, const Function &function);
};

struct Statement {
  enum class StmtType { Function, Return };

  StmtType type;
  union {
    Function func;
    Return retStmt;
  };

  Statement(const Function &func);
  Statement(const Return &ret);
  Statement(const Statement &stmt);
  ~Statement();

  Statement &operator=(const Statement &other);

  static Statement &assign_to_left(Statement &left, const Statement &right);

  friend std::ostream &operator<<(std::ostream &os, const Statement &statement);
};

struct Program {
  std::vector<Statement> statements;

    friend std::ostream &operator<<(std::ostream &os, const Program &program);
};
