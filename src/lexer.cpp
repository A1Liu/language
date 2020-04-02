#include "lexer.h"
#include <cstdlib>
#include <ostream>

struct Escaped {
  const std::string_view str;

  Escaped(const std::string_view &_str) : str(_str) {}

  friend inline std::ostream &operator<<(std::ostream &os, const Escaped &e) {
    for (const char *char_p = e.str.begin(); char_p != e.str.end(); char_p++) {
      switch (*char_p) {
      case '\a':
        os << "\\a";
        break;
      case '\b':
        os << "\\b";
        break;
      case '\f':
        os << "\\f";
        break;
      case '\n':
        os << "\\n";
        break;
      case '\r':
        os << "\\r";
        break;
      case '\t':
        os << "\\t";
        break;
      case '\v':
        os << "\\v";
        break;
      case '\\':
        os << "\\\\";
        break;
      case '\'':
        os << "\\'";
        break;
      case '\"':
        os << "\\\"";
        break;
      case '\?':
        os << "\\\?";
        break;
      default:
        os << *char_p;
      }
    }
    return os;
  }
};

Lexer::Lexer(const std::string &_data) : data(_data) {
  indentation_stack.push_back(0);
}

Token Lexer::next() {
  Token tok;
  next(tok);
  return tok;
}

void Lexer::next(Token &tok) {
  if (state == LexerState::DEDENT) {
    next_tok_dedent(tok);
  } else if (state == LexerState::NORMAL) {
    next_tok_normal(tok);
  } else if (state == LexerState::INDENTATION) {
    next_tok_indent(tok);
  } else if (state == LexerState::END && indentation_stack.size() > 1) {
    tok.type = TokenType::DEDENT;
    indentation_stack.pop_back();
  } else if (state == LexerState::END) {
    tok.type = TokenType::END;
  }
}

void Lexer::next_tok_indent(Token &tok) {

  int indentation_level = 0;
  bool done = false;
  int begin = index;

  for (; !done && index < data.size();) {
    switch (data.at(index)) {
    case '\\':
      index++;
      if (!handle_newline(tok)) {
        tok.view = std::string_view(&data.at(index), 2);
        return;
      }
      begin = index;
      break;
    case '\r':
    case '\n':
      if (!handle_newline(tok)) {
        return;
      }
      indentation_level = 0;
      begin = index;
      break;
    case ' ':
      indentation_level += 1;
      index++;
      break;
    case '\t':
      indentation_level += 8 - indentation_level % 8;
      index++;
      break;
    default:
      done = true;
    }
  }

  int previous_indentation = indentation_stack.back();
  int end = index;
  if (index == data.size()) {
    state = LexerState::END;
    next(tok);
  } else if (indentation_level < previous_indentation) {
    state = LexerState::DEDENT;
    this->indentation_level = indentation_level;
    next_tok_dedent(tok);
  } else if (indentation_level == previous_indentation) {
    state = LexerState::NORMAL;
    next_tok_normal(tok);
  } else if (indentation_level > previous_indentation) {
    tok.type = TokenType::INDENT;
    state = LexerState::NORMAL;
    indentation_stack.push_back(indentation_level);
    tok.view = data.substr(begin, end - begin);
  }
}

void Lexer::next_tok_dedent(Token &tok) {
  int previous_indentation = indentation_stack.back();
  if (indentation_level < previous_indentation) {
    indentation_stack.pop_back();
    if (indentation_level > indentation_stack.back()) {
      tok.type = TokenType::UNKNOWN_DEDENT;
      state = LexerState::NORMAL;
      indentation_stack.push_back(indentation_level);
    } else {
      tok.type = TokenType::DEDENT;
    }
  } else if (indentation_level == previous_indentation) {
    state = LexerState::NORMAL;
    next(tok);
  } else {
    state = LexerState::NORMAL;
    tok.type = TokenType::UNKNOWN_DEDENT;
    indentation_stack.push_back(indentation_level);
  }
}

void Lexer::next_tok_normal(Token &tok) {

  auto advance_single_token = [&tok, this](TokenType token_type) {
    tok.type = token_type;
    tok.view = data.substr(index++, 1);
    if (index == data.size()) {
      state = LexerState::END;
    }
  };

  auto advance_repeated_character = [&tok, this](char repeated_char) {
    uint64_t char_count = 0;
    for (int i = index; i < data.size() && data.at(i) == repeated_char;
         i++, char_count++)
      ;

    tok.view = data.substr(index, char_count);
    index += char_count;
    if (index == data.size()) {
      state = LexerState::END;
    }
    return char_count;
  };

  bool found_word = false;
  while (!found_word) {
    // Ignore leading whitespace
    for (const char *c = &data.at(index); index < data.size();) {
      c = &data.at(index);
      if (*c == '\\') {
        index++;
        handle_newline(tok);
        if (tok.type == TokenType::UNKNOWN) {
          tok.view = std::string_view(c, 2);
          return;
        }
      } else if (*c == ' ' || *c == '\t')
        index++;
      else {
        break;
      }
    }

    if (index == data.size()) {
      state = LexerState::END;
      next(tok);
      return;
    }

    switch (data.at(index)) {
    case '(':
      advance_single_token(TokenType::LPAREN);
      parentheses_count++;
      return;
    case ')':
      advance_single_token(TokenType::RPAREN);
      if (parentheses_count > 0) {
        parentheses_count--;
      }
      return;
    case '*': {
      uint64_t star_count = advance_repeated_character('*');
      if (star_count == 1) {
        tok.type = TokenType::STAR;
      } else if (star_count == 2) {
        tok.type = TokenType::STAR_STAR;
      } else {
        tok.type = TokenType::UNKNOWN;
      }
      return;
    }
    case '>':
    case '<':
    case '=': {
      uint64_t equals_count = advance_repeated_character('=');
      if (equals_count == 1) {
        tok.type = TokenType::EQUALS;
      } else if (equals_count == 2) {
        tok.type = TokenType::EQUALS_EQUALS;
      } else {
        tok.type = TokenType::UNKNOWN;
      }
      return;
    }
    case '%':
      advance_single_token(TokenType::PERCENT);
      return;
    case '/':
      advance_single_token(TokenType::DIV);
      return;
    case '-':
      advance_single_token(TokenType::MINUS);
      return;
    case '+':
      advance_single_token(TokenType::PLUS);
      return;
    case '\r':
    case '\n': // Handle newlines
      handle_newline(tok);
      if (parentheses_count == 0) {
        state = LexerState::INDENTATION;
        return;
      }
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      handle_numeric(tok);
      return;
    default: // It's some kind of keyword or identifier
      if (isalpha(data.at(index))) {
        found_word = true;
      } else { // Or maybe something really wonky we don't recognize
        tok.type = TokenType::UNKNOWN;
        tok.view = data.substr(index++, 1);
        return;
      }
    }
  }

  int begin = index, i;
  for (i = 0; index < data.size() && isalnum(data.at(index)); i++, index++)
    ;

  auto running_string = tok.view = std::string_view(&data.at(begin), i);

  if (running_string == "def") {
    tok.type = TokenType::DEF;
  } else if (running_string == "None") {
    tok.type = TokenType::NONE;
  } else if (running_string == "return") {
    tok.type = TokenType::RETURN;
  } else if (running_string == "pass") {
    tok.type = TokenType::PASS;
  } else {
    tok.type = TokenType::IDENT;
  }

  if (index == data.size()) {
    state = LexerState::END;
  }
}

bool Lexer::handle_newline(Token &tok) {
  tok.type = TokenType::NEWLINE;
  int begin = index;
  if (data.at(index) == '\n') {
    ++index;
    tok.view = data.substr(begin, 1);
  } else if (data.at(index) == '\r' && data.at(index + 1) == '\n') {
    index += 2;
    tok.view = data.substr(begin, 2);
  } else {
    tok.type = TokenType::UNKNOWN;
    index += 1;
    tok.view = data.substr(begin, 1);
    return false;
  }
  return true;
}

void Lexer::handle_numeric(Token &tok) {
  char *begin = const_cast<char *>(&data.at(index));
  int len = 1;
  for (index += 1; data.at(index) >= '0' && data.at(index) <= '9';
       index++, len++)
    ;

  bool found_dot = data.at(index) == '.';

  if (found_dot) {
    for (len += 1, index += 1; data.at(index) >= '0' && data.at(index) <= '9';
         index++, len++)
      ;
  }
  tok.view =
      std::string_view{begin, static_cast<std::string_view::size_type>(len)};

  char *end = begin + len;
  if (found_dot) {
    tok.type = TokenType::FLOATING_POINT;
    tok.floating_value = std::strtod(begin, &end);
  } else {
    tok.type = TokenType::INTEGER;
    tok.integer_value = std::strtol(begin, &end, 10);
  }
}

std::ostream &operator<<(std::ostream &os, const Token &token) {
  os << "type: " << token.type;
  if (token.type == TokenType::INTEGER) {
    os << " value: " << token.integer_value;
  } else if (token.type == TokenType::FLOATING_POINT) {
    os << " value: " << token.floating_value;
  } else {
    os << " view: `" << Escaped(token.view) << '`';
  }
  return os;
}

bool Token::operator==(const Token &rhs) const {
  return type == rhs.type && view == rhs.view;
}

bool Token::operator!=(const Token &rhs) const { return !(rhs == *this); }

std::ostream &operator<<(std::ostream &os, const TokenType &type) {
#define case_macro(token_type)                                                 \
  case TokenType::token_type:                                                  \
    os << #token_type;                                                         \
    break

  switch (type) {
    case_macro(DEF);
    case_macro(PASS);
    case_macro(RETURN);
    case_macro(SWITCH);
    case_macro(CASE);
    case_macro(BREAK);
    case_macro(FOR);
    case_macro(WHILE);
    case_macro(IN);
    case_macro(IS);
    case_macro(CLASS);
    case_macro(RAISE);
    case_macro(YIELD);
    case_macro(ASYNC);
    case_macro(IF);
    case_macro(ELSE);
    case_macro(IDENT);
    case_macro(LPAREN);
    case_macro(RPAREN);
    case_macro(LBRACKET);
    case_macro(RBRACKET);
    case_macro(EQUALS);
    case_macro(EQUALS_EQUALS);
    case_macro(LESS_THAN);
    case_macro(GREATER_THAN);
    case_macro(LESS_EQ);
    case_macro(GREATER_EQ);
    case_macro(ARROW);
    case_macro(DOT);
    case_macro(PLUS);
    case_macro(MINUS);
    case_macro(STAR);
    case_macro(STAR_STAR);
    case_macro(DIV);
    case_macro(DIV_DIV);
    case_macro(PERCENT);
    case_macro(COLON);
    case_macro(NONE);
    case_macro(NEWLINE);
    case_macro(INDENT);
    case_macro(DEDENT);
    case_macro(UNKNOWN_DEDENT);
    case_macro(UNKNOWN);
    case_macro(END);
    case_macro(INTEGER);
    case_macro(FLOATING_POINT);
    case_macro(STRING);
    case_macro(INT_TYPE);
    case_macro(FLOAT_TYPE);
    case_macro(STR_TYPE);
    case_macro(BOOL_TYPE);
  }
  return os;
}
