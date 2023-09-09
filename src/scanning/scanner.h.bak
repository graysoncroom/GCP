#pragma once

#include "token.h"

#include <cctype>
#include <string>
#include <optional>
#include <variant>
#include <fstream> 
#include <stdexcept>
#include <vector>

class Scanner {
public:
  Scanner(std::string);

  char next_character();
  char skip();                              /* Gets the next non-whitespace character from the input stream */
  void put_back(char);                      /* Put a character back into the input stream */
  int scan_integer_literal(char);           /* Scan integer literals into a buffer and parse them into int objects */
  void scan_file();                         /* Scans a file and prints out its Tokens */

  /* Scan the next token */
  template <TokenType T>
  Token<T> scan();

private:
  std::string filename; 
  std::ifstream file;
  char put_back_buffer;
  unsigned line_number;
};

/*
template <TokenType T>
Token<T> Scanner::scan() {
  char c = skip();

  // Check for EOF
  if (c == '\0') { return std::optional(std::nullopt); }

  std::vector<TokenType> all_symbolic_token_types =
    { TokenType::Plus
    , TokenType::Minus
    , TokenType::Star
    , TokenType::Slash };
  std::vector<TokenType> possible_token_types;

  //for (TokenType &token_type : all_symbolic_token_types) {
  //  if (Token::tttos(token_type)[0] == c) {
  //    return Token<token_type>;
  //  }
  //}

  if (possible_token_types.size() == 0) {
    if (std::isdigit(c)) {
      return Token<TokenType::IntegerLiteral>(std::atoi(c));
    } else {
      throw std::logic_error("cannot construct token");
    }
  }

  //return Token<possible_token_types[0]>();
  return Token<TokenType::Plus>();
}
*/
