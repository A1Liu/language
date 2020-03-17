#include "parser.h"
#include <iostream>

Parser::Parser(Lexer &lexer) {
  Token tok;
  tokens.push_back(tok = lexer.next());
  std::cout << tok << std::endl;
  while (tok.type != TokenType::END) {
    tokens.push_back(tok = lexer.next());
    std::cout << tok << std::endl;
  }
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
    return true;
  }

  if (type == TokenType::DEF) {
    return try_parse_function(std::get<Function>(statement.data = Function()));
  } else if (type == TokenType::RETURN) {
    return try_parse_return(std::get<Return>(statement.data = Return()));
  }

  return false;
}

bool Parser::try_parse_function(Function &func) {

  auto assert_bump = [this](TokenType type) {
    if (pop().type != type) {
      return false;
    } else {
      return true;
    }
  };

  if (!assert_bump(TokenType::DEF)) {
    return false;
  }

  func.name = peek().view;
  if (pop().type != TokenType::IDENT) {
    return false;
  }

  if (pop().type != TokenType::LPAREN) {
    return false;
  }

  if (pop().type != TokenType::RPAREN) {
    return false;
  }

  if (pop().type != TokenType::COLON) {
    return false;
  }

  if (pop().type != TokenType::NEWLINE) {
    return false;
  }

  if (pop().type != TokenType::INDENT) {
    return false;
  }

  Statement stmt;
  bool result = try_parse_statement(stmt);
  if (result)
    func.statements.push_back(stmt);
  else
    return false;

  while (peek().type != TokenType::DEDENT &&
         (result = try_parse_statement(stmt))) {
    func.statements.push_back(std::move(stmt));
  }

  if (!assert_bump(TokenType::DEDENT)) {
    return false;
  }
  return true;
}

bool Parser::try_parse_return(Return &ret) {
  if (pop().type != TokenType::RETURN) {
    return false;
  }

  Expression expr;
  if (!try_parse_expr(expr)) {
    return false;
  }

  return pop().type == TokenType::NEWLINE;
}

bool Parser::try_parse_expr(Expression &expr) {
  if (peek().type == TokenType::NONE) {
    pop();
    return true;
  }
  return false;
}

bool Parser::try_parse_binary_expr(Expression &expr) { return false; }

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
    return try_parse_atom_expr(expr);
  }
  pop();
  Expression *next_expr = new Expression();

  if (!try_parse_atom_expr(*next_expr)) {
    return false;
  }
  expr.expr = UnaryOp(uoptype, next_expr);
  return true;
}

bool Parser::try_parse_atom_expr(Expression &expr) {
  Token tok = peek();

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
  default:
    return false;
  }
}
