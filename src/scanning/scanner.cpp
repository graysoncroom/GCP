#include "scanner.h"

Scanner::Scanner(std::string fn) 
  : filename(fn), file(fn), put_back_buffer('\0'), line_number(1) { }

char Scanner::next_character() {
  // If we have a character in the putback buffer,
  // we want to read that first, then empty the
  // buffer
  if (put_back_buffer != '\0') {
    char c = put_back_buffer;
    put_back_buffer = '\0';
    return c;
  }

  // Otherwise, we read a single character from
  // the open input file, and conditionally
  // increment our rudimentary line counter
  char c;
  file >> c;
  line_number += (c == '\n');

  return c;
}

/*
Gets the next non-whitespace character from the input stream

Returns:
    char: The next non-whitespace character from the input stream
*/
char Scanner::skip() {
  char c;
  do {
    c = next_character();
  } while (c == ' ');

  return c;
}

/*
Put a character back into the input stream

Args:
    c (char): Character to put back into the input stream
*/
void Scanner::put_back(char c) {
  put_back_buffer = c;
}

/*
Scan integer literals into a buffer and parse them into int objects

Args:
    c (char): Current character from input stream

Returns:
    int: Scanned integer literal
*/
int Scanner::scan_integer_literal(char c) {
  std::string str = "";

  while (std::isdigit(c)) {
    str += c;
    c = next_character();
  }

  put_back(c);

  return std::stoi(str);
}
  
/*
Scan the next token

Args:
    current_token (Token): Global Token object to scan data into

Raises:
  std::logic_error: If an unrecognized Token is reached

Returns:
    bool: True if a Token was read, False if EOF was reached
*/
/*
std::optional<Token::AnyToken> Scanner::scan() {
  char c = skip();

  // Check for EOF
  if (c == '') { return std::nullopt; }

  std::vector<TokenType> all_symbolic_token_types =
    { TokenType::Plus
    , TokenType::Minus
    , TokenType::Star
    , TokenType::Slash };
  std::vector<Token> possible_token_types;

  for (TokenType &token_type : all_symbolic_token_types) {
    if (tttos(token_type)[0] == c) {
      possible_token_types.push_back(Token<decltype(token_type)>());
    }
  }

  if (possible_token_types.size() == 0) {
    if (std::isdigit(c)) {
      return Token<IntegerLiteral>(std::ctoi(c));
    } else {
      throw std::logic_error("cannot construct token");
    }
  }

  return possible_token_types[0];
}
*/

/*
Scans a file and prints out its Tokens
*/
/*
void Scanner::scan_file() {
  while ((std::optional<Token> token = scan()).hasValue()) {
    std::cout << token << ' ';
  }
}
*/
