#pragma once
#include "util.h"
#include <cstdint>
#include <ostream>
#include <stdbool.h>
#include <unordered_map>
#include <vector>

enum class TokenType {
  DEF,
  PASS,
  RETURN,
  SWITCH,
  CASE,
  BREAK,
  FOR,
  WHILE,
  IN,
  IS,
  CLASS,
  RAISE,
  YIELD,
  ASYNC,
  IF,
  ELSE,
  IDENT,
  LPAREN,
  RPAREN,
  LBRACKET,
  RBRACKET,
  NOT_EQUAL,
  EQUALS,
  EQUALS_EQUALS,
  LESS_THAN,
  GREATER_THAN,
  LESS_EQ,
  GREATER_EQ,
  ARROW,
  DOT,
  PLUS,
  MINUS,
  STAR,
  STAR_STAR,
  DIV,
  DIV_DIV,
  PERCENT,
  COLON,
  COMMA,
  NONE,
  NEWLINE,
  INDENT,
  DEDENT,
  UNKNOWN_DEDENT,
  UNKNOWN,
  END,
  INTEGER,
  FLOATING_POINT,
  STRING,
  TRIPLE_DASH,
  INT_TYPE,
  FLOAT_TYPE,
  STR_TYPE,
  BOOL_TYPE
};

enum LexerState { NORMAL, INDENTATION, DEDENT, END };

struct Token {

  TokenType type;
  union {
    double floating_value;
    uint64_t integer_value;
    struct {
      uint8_t identifier_len;
      char identifier[23];
    };
    String view;
  };

  bool operator==(const Token &rhs) const;
  bool operator!=(const Token &rhs) const;
};

struct Lexer {

  String data;
  std::vector<uint32_t> indentation_stack;
  uint32_t index = 0;
  uint16_t indentation_level = 0;
  uint8_t parentheses_count = 0;
  uint8_t state = LexerState::INDENTATION;

  explicit Lexer(const char *data);

  Token next();
  void next(Token &tok);

  void next_tok_normal(Token &tok);
  void next_tok_indent(Token &tok);
  void next_tok_dedent(Token &tok);
  void handle_numeric(Token &tok);
  bool handle_newline(Token &tok);
};

std::ostream &operator<<(std::ostream &os, const Token &);
std::ostream &operator<<(std::ostream &os, const TokenType &);
