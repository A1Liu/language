#include <ostream>
#include <string_view>
#include <variant>
#include <vector>

struct Expression;
struct Function;
struct Return;
struct Statement;

struct NoneExpr {
  friend std::ostream &operator<<(std::ostream &os, const NoneExpr &);
};

struct IntegerLiteral {};

struct FloatingLiteral {};

struct StringLiteral {};

struct Identifier {};

struct FunctionCall {};

enum class BinaryOpType { Plus, Multply, Divide, Add };

enum class UnaryOpType {};

struct BinaryOp {
  BinaryOpType type;
  Expression *left;
  Expression *right;
};

struct UnaryOp {
  UnaryOpType type;
  Expression *operand;
};

struct Expression {
  std::variant<NoneExpr, IntegerLiteral, FloatingLiteral, StringLiteral,
               FunctionCall, Identifier, BinaryOpType, UnaryOpType>
      expr = NoneExpr();

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

  friend std::ostream &operator<<(std::ostream &os, const Function &func);
};

struct Pass {
  friend std::ostream &operator<<(std::ostream &os, const Pass &pass);
};

struct Statement {
  std::variant<Function, Return, Pass> data;

  Statement();
  Statement(const Function &func);
  Statement(const Return &ret);

  static Statement &assign_to_left(Statement &left, const Statement &right);

  friend std::ostream &operator<<(std::ostream &os, const Statement &statement);
};

struct Program {
  std::vector<Statement> statements;

  friend std::ostream &operator<<(std::ostream &os, const Program &program);
};
