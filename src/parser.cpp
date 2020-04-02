#include "parser.h"

// Propogate
#define prop(expr)                                                             \
  if (!(expr)) {                                                               \
    return false;                                                              \
  }

Parser::Parser(Lexer &lexer) {
  Token tok;
  do
    tokens.push_back(tok = lexer.next());
  while (tok.type != TokenType::END);
}

const Token &Parser::peek() { return tokens.at(index); }
Token &Parser::pop() { return tokens.at(index++); }
int Parser::idx() { return index; }
void Parser::reset(int idx) { index = idx; }

bool Parser::try_parse_program(Program &program) {
  Statement stmt;
  bool result = true;
  while (peek().type != TokenType::END &&
         (result = try_parse_statement(stmt))) {
    program.statements.push_back(std::move(stmt));
  }

  return result;
}

bool Parser::try_parse_statement(Statement &statement) {
  TokenType type = peek().type;
  if (type == TokenType::PASS) { // Do nothing
    pop();
    return pop().type == TokenType::NEWLINE;
  }

  if (type == TokenType::DEF) {
    return try_parse_function(std::get<Function>(statement.data = Function()));
  } else if (type == TokenType::RETURN) {
    return try_parse_return(std::get<Return>(statement.data = Return()));
  }

  return false;
}

bool Parser::try_parse_function(Function &func) {

  if (pop().type != TokenType::DEF) {
    return false;
  }

  func.name = peek().view;
  prop(pop().type == TokenType::IDENT);
  prop(pop().type == TokenType::LPAREN);
  prop(pop().type == TokenType::RPAREN);
  prop(pop().type == TokenType::COLON);
  prop(pop().type == TokenType::NEWLINE);
  prop(pop().type == TokenType::INDENT);

  Statement stmt;
  bool result = try_parse_statement(stmt);
  if (result) {
    func.statements.push_back(stmt);
  } else
    return false;

  while (peek().type != TokenType::DEDENT &&
         (result = try_parse_statement(stmt))) {
    func.statements.push_back(std::move(stmt));
  }

  prop(pop().type == TokenType::DEDENT);

  return true;
}

bool Parser::try_parse_return(Return &ret) {
  prop(pop().type == TokenType::RETURN);
  prop(try_parse_expr(ret.expr));

  return pop().type == TokenType::NEWLINE;
}

bool Parser::try_parse_expr(Expression &expr) {
  Expression left;
  prop(try_parse_unary_expr(left));

  BinaryOpType boptype;
  switch (peek().type) {
  case TokenType::PLUS:
    boptype = BinaryOpType::Plus;
    break;
  case TokenType::MINUS:
    boptype = BinaryOpType::Minus;
    break;
  case TokenType::STAR:
    boptype = BinaryOpType::Multiply;
    break;
  case TokenType::DIV_DIV:
    boptype = BinaryOpType::DoubleDivide;
    break;
  case TokenType::PERCENT:
    boptype = BinaryOpType::Modulo;
    break;
  default:
    expr = left;
    return true;
  }
  pop();

  Expression *right = new Expression();
  prop(try_parse_unary_expr(*right));

  expr.expr = BinaryOp(boptype, new Expression(left), right);

  return true;
}

bool Parser::try_parse_unary_expr(Expression &expr) {
  TokenType type = peek().type;
  UnaryOpType uoptype;

  switch (type) {
  case TokenType::MINUS:
    uoptype = UnaryOpType::Negate;
    break;
  case TokenType::STAR:
    uoptype = UnaryOpType::Splat;
    break;
  case TokenType::STAR_STAR:
    uoptype = UnaryOpType::DoubleSplat;
    break;
  default:
    return try_parse_postfix_expr(expr);
  }
  pop();
  Expression *next_expr = new Expression();

  prop(try_parse_postfix_expr(*next_expr));

  expr.expr = UnaryOp(uoptype, next_expr);
  return true;
}

// @TODO Brackets
bool Parser::try_parse_postfix_expr(Expression &expr) {
  prop(try_parse_atom_expr(expr));
  return true;
}

bool Parser::try_parse_atom_expr(Expression &expr) {
  Token tok = pop();

  switch (tok.type) {
  case TokenType::IDENT:
    expr.expr = Identifier(tok.view);
    return true;
  case TokenType::INTEGER:
    expr.expr = IntegerLiteral(tok.integer_value);
    return true;
  case TokenType::FLOATING_POINT:
    expr.expr = FloatLiteral(tok.floating_value);
    return true;
  case TokenType::NONE:
    expr.expr = NoneExpr();
    return true;
  default: // @TODO dictionary/list literals, slice literals, accessors,
           // function calls
    return false;
  }
}
