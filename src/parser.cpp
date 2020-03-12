#include "parser.h"

Parser::Parser(Lexer &lexer) {
  while (lexer.has_next()) {
    tokens.push_back(lexer.next());
  }
}

bool Parser::try_parse_program(Program &program) {

  Statement s{Function()};

  program.statements.push_back(s);

  return try_parse_function(program.statements.back().func);
}

bool Parser::try_parse_function(Function &func) {

  int begin = index;

  auto assert_bump = [this, begin](TokenType type) {
    if (tokens.at(index).type != TokenType::DEF) {
      index = begin;
      return false;
    } else {
      index++;
      return true;
    }
  };

  if (!assert_bump(TokenType::DEF)) {
    return false;
  }
  if (!assert_bump(TokenType::IDENT)) {
    return false;
  }
  func.name = tokens.at(index).view;

  if (!assert_bump(TokenType::LPAREN)) {
    return false;
  }

  if (!assert_bump(TokenType::RPAREN)) {
    return false;
  }

  if (!assert_bump(TokenType::COLON)) {
    return false;
  }

  if (!assert_bump(TokenType::NEWLINE)) {
    return false;
  }

  if (!assert_bump(TokenType::INDENT)) {
    return false;
  }

  Return ret;
  if (!try_parse_return(ret))
    return false;

  if (!assert_bump(TokenType::DEDENT)) {
    return false;
  }

  return true;
}

bool Parser::try_parse_return(Return &ret) {
  if (tokens.at(index).type != TokenType::RETURN ||
      tokens.at(index + 1).type != TokenType::NONE)
    return false;
  index += 2;
  return true;
}
