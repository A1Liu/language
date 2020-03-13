#include "parser.h"

Parser::Parser(Lexer &lexer) {
  while (lexer.has_next()) {
    tokens.push_back(lexer.next());
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
  if (!try_parse_expression(expr)) {
    return false;
  }

  return pop().type == TokenType::NEWLINE;
}

bool Parser::try_parse_expression(Expression &expr) {
  if (peek().type == TokenType::NONE) {
    pop();
    return true;
  }
  return false;
}
