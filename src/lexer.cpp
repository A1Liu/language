#include "lexer.h"
#include <iostream>

Lexer::Lexer(std::string &&_data) : data(_data) {
  indentation_stack.push_back(0);
}

bool Lexer::has_next() {
  return state != LexerState::END || indentation_count > 0;
}

Lexer::Token Lexer::next() {
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
    tok.data = nullptr;
    tok.begin = index;
    tok.end = index;
  } else if (state == LexerState::NORMAL) {
    next_tok_normal(tok);
  } else if (state == LexerState::INDENTATION) {
    next_tok_indent(tok);
  }
}

void Lexer::next_tok_indent(Token &tok) {

  int indentation_level = 0;
  bool done = false;
  tok.begin = index;
  tok.data = nullptr;

  for (; !done && index < data.size();) {
    switch (data.at(index)) {
    case '\r':
    case '\n':
      if (!handle_newline(tok)) {
        return;
      }
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
  tok.end = index;
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
    tok.data = nullptr;
    tok.begin = index;
    tok.end = index;
    return;
  }

  tok.begin = index;

  auto advance_single_token = [&tok, this](TokenType token_type) {
    tok.type = token_type;
    tok.end = ++index;
    tok.data = nullptr;
    if (index == data.size()) {
      state == LexerState::END;
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
    std::cerr << "got a number" << std::endl;
  default: // It's some kind of keyword or identifier
    break;
  }

  tok.end = index;

  for (char c; index < data.size() && isalpha(c = data.at(index));
       running_string.push_back(c), tok.end = ++index) {
  }

  if (running_string == "def") {
    tok.type = TokenType::DEF;
  } else if (running_string == "None") {
    tok.type = TokenType::NONE;
  } else if (running_string == "return") {
    tok.type = TokenType::RETURN;
  } else {
    tok.type = TokenType::IDENT;
  }

  if (index == data.size()) {
    state = LexerState::END;
  }
  running_string.clear();
}

bool Lexer::handle_newline(Token &tok) {
  tok.type = TokenType::NEWLINE;
  tok.data = nullptr;
  tok.begin = index;
  if (data.at(index) == '\n') {
    tok.end = ++index;
  } else if (data.at(index) == '\r' && data.at(index + 1) == '\n') {
    index += 2;
    tok.end = index;
  } else {
    tok.type = TokenType::UNKNOWN;
    index += 2;
    tok.end = index;
    return false;
  }
  return true;
}

std::ostream &operator<<(std::ostream &os, const Lexer::Token &token) {
  os << "type: " << token.type << " data: " << token.data
     << " begin: " << token.begin << " end: " << token.end;
  return os;
}

std::ostream &operator<<(std::ostream &os, const Lexer::TokenType &type) {
  switch (type) {
  case Lexer::TokenType::DEF:
    os << "DEF";
    break;
  case Lexer::TokenType::IDENT:
    os << "IDENT";
    break;
  case Lexer::TokenType::LPAREN:
    os << "LPAREN";
    break;
  case Lexer::TokenType::RPAREN:
    os << "RPAREN";
    break;
  case Lexer::TokenType::COLON:
    os << "COLON";
    break;
  case Lexer::TokenType::RETURN:
    os << "RETURN";
    break;
  case Lexer::TokenType::NONE:
    os << "NONE";
    break;
  case Lexer::TokenType::NEWLINE:
    os << "NEWLINE";
    break;
  case Lexer::TokenType::INDENT:
    os << "INDENT";
    break;
  case Lexer::TokenType::DEDENT:
    os << "DEDENT";
    break;
  case Lexer::TokenType::UNKNOWN_DEDENT:
    os << "UNKNOWN_DEDENT";
    break;
  case Lexer::TokenType::UNKNOWN:
    os << "UNKNOWN";
    break;
  case Lexer::TokenType::END:
    os << "END";
    break;
  }
  return os;
}
