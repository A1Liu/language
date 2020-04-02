#include "parser.h"
#include <iostream>

// Propogate
#define prop(expr)                                                             \
  if (!(expr)) {                                                               \
    return false;                                                              \
  }

#define prop_err(expr, str_view, message)                                      \
  if (!(expr)) {                                                               \
    errors.push_back(ParseError{str_view, message});                           \
    return false;                                                              \
  }

Parser::Parser(Lexer *lexer) {
  Token tok;
  do
    tokens.push_back(tok = lexer->next());
  while (tok.type != TokenType::END);
}

const Token &Parser::peek() { return tokens.at(index); }
const Token &Parser::pop() { return tokens.at(index++); }

bool Parser::try_parse_program(Program &program) {
  Stmt stmt;
  bool result = true;
  while (peek().type != TokenType::END &&
         (result = try_parse_statement(stmt))) {
    program.statements.push_back(std::move(stmt));
  }

  return result;
}

bool Parser::try_parse_statement(Stmt &stmt) {
  TokenType type = peek().type;
  if (type == TokenType::PASS) { // Do nothing
    pop();
    return pop().type == TokenType::NEWLINE;
  }

  Expr expr;
  prop(try_parse_expr(expr));

  if (peek().type == TokenType::NEWLINE) {
    stmt.type = StmtType::Expr;
    stmt.expr = expr;
    pop();
    return true;
  }
  std::cout << "hi" << std::endl;

  Token tok = pop();
  prop_err(tok.type == TokenType::EQUALS, tok.view,
           "unrecognized location while trying to parse a statement");

  stmt.type = StmtType::Assign;
  stmt.assign_target = expr;
  prop(try_parse_expr(expr));
  stmt.assign_value = expr;
  tok = pop();
  prop_err(tok.type == TokenType::NEWLINE, tok.view,
           "expected newline after assignment");
  return true;
}

bool Parser::try_parse_expr(Expr &expr) { return try_parse_add(expr); }

bool Parser::try_parse_add(Expr &expr) {
  prop(try_parse_mul(expr));
  const TokenType type = peek().type;
  ExprType expr_type;
  if (type == TokenType::PLUS) {
    expr_type = ExprType::Add;
  } else if (type == TokenType::MINUS) {
    expr_type = ExprType::Sub;
  } else {
    return true;
  }

  pop();

  Expr *left = new Expr();
  Expr *right = new Expr();
  *left = expr;

  prop(try_parse_mul(*right));
  expr = Expr(expr_type, left, right);
  return true;
}

bool Parser::try_parse_mul(Expr &expr) {
  prop(try_parse_unary_prefix(expr));
  TokenType type = peek().type;
  ExprType expr_type;

  if (type == TokenType::STAR) {
    expr_type = ExprType::Mul;
  } else if (type == TokenType::DIV) {
    expr_type = ExprType::Div;
  } else if (type == TokenType::PERCENT) {
    expr_type = ExprType::Mod;
  } else {
    return true;
  }

  pop();

  Expr *left = new Expr();
  Expr *right = new Expr();
  *left = expr;

  prop(try_parse_unary_prefix(*right));
  expr = Expr(expr_type, left, right);
  return true;
}

bool Parser::try_parse_unary_prefix(Expr &expr) {
  return try_parse_unary_postfix(expr);
}

bool Parser::try_parse_unary_postfix(Expr &expr) {
  return try_parse_atom_expr(expr);
}

bool Parser::try_parse_atom_expr(Expr &expr) {
  Token tok = pop();

  switch (tok.type) {
  case TokenType::IDENT:
    expr = tok.view;
    break;
  case TokenType::INTEGER:
    expr = tok.integer_value;
    break;
  case TokenType::FLOATING_POINT:
    expr = tok.floating_value;
    break;
  case TokenType::NONE:
    expr = ExprType::None;
    break;
  case TokenType::LPAREN: {
    prop(try_parse_expr(expr));
    const Token &tok = pop();
    prop_err(tok.type != TokenType::RPAREN, tok.view,
             "expected a closing paren");
    break;
  }
  default: // @TODO dictionary/list literals, slice literals, accessors,
           // function calls
    return false;
  }

  return true;
}