#include "parser.h"

// Propogate
#define prop(expr)                                                             \
  if (!(expr)) {                                                               \
    return false;                                                              \
  }

// Propogate and error out
#define prop_err(expr, str_view, message)                                      \
  if (!(expr)) {                                                               \
    error = ParseError{str_view, message};                                     \
    return false;                                                              \
  }

Parser::Parser(BucketArray *_buckets, const char *data)
    : lexer(data), buckets(_buckets) {
  Token tok;
  do
    tokens.push_back(tok = lexer.next());
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

  Token tok = pop();
  prop_err(tok.type == TokenType::EQUALS, tok.view,
           "unrecognized token while trying to parse an expression");

  stmt.type = StmtType::Assign;
  stmt.assign_target = expr;
  prop(try_parse_expr(expr));
  stmt.assign_value = expr;
  tok = pop();
  prop_err(tok.type == TokenType::NEWLINE, tok.view,
           "expected newline after assignment");
  return true;
}

bool Parser::try_parse_expr(Expr &expr) { return try_parse_compare(expr); }

bool Parser::try_parse_compare(Expr &expr) {
  prop(try_parse_add(expr));

  while (true) {
    const TokenType type = peek().type;
    ExprType expr_type;
    switch (type) {
    case TokenType::EQUALS_EQUALS:
      expr_type = ExprType::Eq;
      break;
    case TokenType::NOT_EQUAL:
      expr_type = ExprType::Neq;
      break;
    case TokenType::LESS_EQ:
      expr_type = ExprType::Leq;
      break;
    case TokenType::GREATER_EQ:
      expr_type = ExprType::Geq;
      break;
    case TokenType::GREATER_THAN:
      expr_type = ExprType::Gt;
      break;
    case TokenType::LESS_THAN:
      expr_type = ExprType::Lt;
      break;
    default:
      return true;
    }

    pop();

    Expr *left = buckets->add<Expr>();
    Expr *right = buckets->add<Expr>();
    *left = expr;

    prop(try_parse_add(*right));
    expr = Expr(expr_type, left, right);
  }
}

bool Parser::try_parse_add(Expr &expr) {
  prop(try_parse_mul(expr));

  while (true) {
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

    Expr *left = buckets->add<Expr>();
    Expr *right = buckets->add<Expr>();
    *left = expr;

    prop(try_parse_mul(*right));
    expr = Expr(expr_type, left, right);
  }
  return true;
}

bool Parser::try_parse_mul(Expr &expr) {
  prop(try_parse_unary_prefix(expr));

  while (true) {
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

    Expr *left = buckets->add<Expr>();
    Expr *right = buckets->add<Expr>();
    *left = expr;

    prop(try_parse_unary_prefix(*right));
    expr = Expr(expr_type, left, right);
  }
}

bool Parser::try_parse_unary_prefix(Expr &expr) {
  return try_parse_unary_postfix(expr);
}

bool Parser::try_parse_unary_postfix(Expr &expr) {
  prop(try_parse_atom_expr(expr)); // brackets and function calls

  if (peek().type != TokenType::LPAREN) {
    return true;
  }

  Expr *callee = buckets->add<Expr>();
  *callee = expr;
  Expr tup;
  prop(try_parse_atom_expr(tup));

  expr.type = ExprType::Call;
  expr.callee = callee;
  if (tup.type == ExprType::Tup) {
    expr.call_tup_begin = tup.tup_begin;
    expr.call_tup_end = tup.tup_end;
  } else {
    expr.call_tup_begin = buckets->add<Expr>();
    *expr.call_tup_begin = tup;
    expr.call_tup_end = expr.call_tup_begin + 1;
  }
  return true;
}

bool Parser::try_parse_atom_expr(Expr &expr) {
  Token tok = pop();

  switch (tok.type) {
  case TokenType::IDENT:
    expr.type = ExprType::Ident;
    expr.ident = tok.view;
    break;
  case TokenType::INTEGER:
    expr.type = ExprType::Int;
    expr.int_value = tok.integer_value;
    break;
  case TokenType::FLOATING_POINT:
    expr.type = ExprType::Float;
    expr.float_value = tok.floating_value;
    break;
  case TokenType::NONE:
    expr = ExprType::None;
    break;
  case TokenType::LPAREN: {
    if (peek().type == TokenType::RPAREN) {
      pop();
      expr.type = ExprType::Tup;
      expr.tup_begin = expr.tup_end = nullptr;
      break;
    }

    prop(try_parse_expr(expr));
    if (peek().type == TokenType::RPAREN) {
      pop();
      break;
    }

    Token tok;
    Expr tuple_expr;
    Pool pool(buckets, 128);
    *pool.add_extend<Expr>(buckets) = expr;

    while ((tok = pop()).type == TokenType::COMMA) {
      prop(try_parse_expr(tuple_expr));
      *pool.add_extend<Expr>(buckets) = tuple_expr;
    }

    prop_err(tok.type == TokenType::RPAREN, tok.view,
             "expected a closing paren");
    auto block = pool.push_to_buckets(buckets);
    expr.type = ExprType::Tup;
    expr.tup_begin = (Expr *)block.begin;
    expr.tup_end = (Expr *)block.progress;
    break;
  }
  default: // @TODO dictionary/list literals, slice literals, accessors
    prop_err(false, tok.view,
             "unrecognized token while trying to parse expression");
    return false;
  }

  // do dot accessor stuff

  return true;
}