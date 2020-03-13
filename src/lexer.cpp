#include "lexer.h"
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

bool Lexer::has_next() {
  return state != LexerState::END || indentation_count > 0;
}

Token Lexer::next() {
  Token tok;
  next(tok);
  return tok;
}

void Lexer::next(Token &tok) {
  if (state == LexerState::DEDENT && indentation_count == 0) {
    state = LexerState::NORMAL;
    next_tok_normal(tok);
  } else if (indentation_count > 0 &&
             (state == LexerState::DEDENT || state == LexerState::END)) {
    indentation_count--;
    tok.type = TokenType::DEDENT;
  } else if (state == LexerState::NORMAL) {
    next_tok_normal(tok);
  } else if (state == LexerState::INDENTATION) {
    next_tok_indent(tok);
  }
}

void Lexer::next_tok_indent(Token &tok) {

  int indentation_level = 0;
  bool done = false;
  int begin = index;

  for (; !done && index < data.size();) {
    switch (data.at(index)) {
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
  if (indentation_level < previous_indentation) {
    for (indentation_count = 0;
         indentation_level < (previous_indentation = indentation_stack.back());
         indentation_stack.pop_back(), indentation_count++) {
    }

    if (indentation_level == previous_indentation) {
      indentation_count--;
      state = LexerState::DEDENT;
      tok.type = TokenType::DEDENT;
    } else {
      tok.type = TokenType::UNKNOWN_DEDENT;
    }
    tok.view = data.substr(begin, end - begin);
  } else if (index == data.size()) {
    tok.type = TokenType::END;
    state = LexerState::END;
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

void Lexer::next_tok_normal(Token &tok) {

  // Ignore leading whitespace
  for (char c;
       index < data.size() && ((c = data.at(index)) == ' ' || c == '\t');
       index++) {
  }

  if (index == data.size()) {
    state = LexerState::END;
    tok.type = TokenType::END;
    return;
  }

  int begin = index;

  auto advance_single_token = [&tok, begin, this](TokenType token_type) {
    tok.type = token_type;
    ++index;
    tok.view = data.substr(begin, 1);
    if (index == data.size()) {
      state = LexerState::END;
    }
  };

  switch (data.at(index)) {
  case '(':
    advance_single_token(TokenType::LPAREN);
    return;
  case ')':
    advance_single_token(TokenType::RPAREN);
    return;
  case ':':
    advance_single_token(TokenType::COLON);
    return;
  case '\r':
  case '\n': // Handle newlines
    state = LexerState::INDENTATION;
    handle_newline(tok);
    return;
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
    throw 0;
  default: // It's some kind of keyword or identifier
    break;
  }

  int end = index;

  for (char c; index < data.size() && isalpha(c = data.at(index));
       running_string.push_back(c), end = ++index) {
  }

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

  tok.view = data.substr(begin, end - begin);
  running_string.clear();
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
    index += 2;
    tok.view = data.substr(begin, 2);
    return false;
  }
  return true;
}

std::ostream &operator<<(std::ostream &os, const Token &token) {
  os << "type: " << token.type << " view: `" << Escaped(token.view) << '`';
  return os;
}

bool Token::operator==(const Token &rhs) const {
  return type == rhs.type && view == rhs.view;
}

bool Token::operator!=(const Token &rhs) const { return !(rhs == *this); }

std::ostream &operator<<(std::ostream &os, const TokenType &type) {
  switch (type) {
  case TokenType::DEF:
    os << "DEF";
    break;
  case TokenType::IDENT:
    os << "IDENT";
    break;
  case TokenType::LPAREN:
    os << "LPAREN";
    break;
  case TokenType::RPAREN:
    os << "RPAREN";
    break;
  case TokenType::COLON:
    os << "COLON";
    break;
  case TokenType::RETURN:
    os << "RETURN";
    break;
  case TokenType::NONE:
    os << "NONE";
    break;
  case TokenType::NEWLINE:
    os << "NEWLINE";
    break;
  case TokenType::INDENT:
    os << "INDENT";
    break;
  case TokenType::DEDENT:
    os << "DEDENT";
    break;
  case TokenType::UNKNOWN_DEDENT:
    os << "UNKNOWN_DEDENT";
    break;
  case TokenType::UNKNOWN:
    os << "UNKNOWN";
    break;
  case TokenType::END:
    os << "END";
    break;
  case TokenType::PASS:
    os << "PASS";
    break;
  }
  return os;
}
