#include <cstdint>
#include <ostream>
#include <stdbool.h>
#include <string>
#include <vector>

struct Lexer {

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

    Lexer::TokenType type;
    void *data;
    int begin;
    int end; // exclusive on end
  };

  std::string data;
  std::string running_string;
  std::vector<int> indentation_stack;
  uint32_t index = 0;
  uint16_t state = LexerState::NORMAL;
  uint16_t indentation_count = 0;

  explicit Lexer(std::string &&data);

  bool has_next();
  Token next();
  void next(Token &tok);

  void next_tok_normal(Token &tok);
  void next_tok_indent(Token &tok);
  bool handle_newline(Token &tok);
};

std::ostream &operator<<(std::ostream &os, const Lexer::TokenType &type);
