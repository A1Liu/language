#pragma once
#include <cstdint>
#include <ostream>
#include <stdbool.h>
#include <string>
#include <vector>

enum class TokenType {
  DEF,
  IDENT,
  LPAREN,
  RPAREN,
  COLON,
  RETURN,
  NONE,
  NEWLINE,
  INDENT,
  DEDENT,
  UNKNOWN_DEDENT,
  UNKNOWN,
  END
};

enum LexerState { NORMAL, INDENTATION, DEDENT, END };

struct Token {

  friend std::ostream &operator<<(std::ostream &os, const Token &token);

  TokenType type;
  void *data;
  std::string_view view;

  bool operator==(const Token &rhs) const;

  bool operator!=(const Token &rhs) const;
};

struct Lexer {

  std::string_view data;
  std::string running_string;
  std::vector<int> indentation_stack;
  uint32_t index = 0;
  uint16_t state = LexerState::INDENTATION;
  uint16_t indentation_count = 0;

  explicit Lexer(const std::string &data);

  bool has_next();
  Token next();
  void next(Token &tok);

  void next_tok_normal(Token &tok);
  void next_tok_indent(Token &tok);
  bool handle_newline(Token &tok);
};

std::ostream &operator<<(std::ostream &os, const TokenType &type);
