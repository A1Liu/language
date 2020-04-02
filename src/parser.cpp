#include "parser.h"

// Propogate
#define prop(expr)                                                             \
  if ((expr)) {                                                                \
    return false;                                                              \
  }

Parser::Parser(BucketArray *_buckets, Lexer *lexer) : buckets(_buckets) {
  Token tok;
  do
    tokens.push_back(tok = lexer->next());
  while (tok.type != TokenType::END);
}

const Token &Parser::peek() { return tokens.at(index); }
Token &Parser::pop() { return tokens.at(index++); }

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

  stmt.type = StmtType::Expr;
  prop(try_parse_atom_expr(stmt.expr));
  return pop().type == TokenType::NEWLINE;
}

bool Parser::try_parse_atom_expr(Expr &expr) {
  Token tok = pop();

  switch (tok.type) {
  case TokenType::IDENT:
    expr = tok.view;
    return true;
  case TokenType::INTEGER:
    expr = tok.integer_value;
    return true;
  case TokenType::FLOATING_POINT:
    expr = tok.floating_value;
    return true;
  case TokenType::NONE:
    expr = ExprType::None;
    return true;
  default: // @TODO dictionary/list literals, slice literals, accessors,
           // function calls
    return false;
  }
}
