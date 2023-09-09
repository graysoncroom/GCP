#include "scanning/token.h"
#include "utils/arguments.h"

#include <string>
#include <fstream>
#include <iostream>

int main(int argc, char **argv) {
  auto args = get_args(argc, argv);

  // Check if the input file exists
  std::string input_filename = args["input"].as<std::string>();
  std::ifstream program_to_compile(input_filename);
  if (!program_to_compile) {
    std::cerr << "Error: Unable to open input file: " << input_filename << std::endl;
    return 1;
  }

  std::vector<Token::Token> scanned_tokens;
  int unknown_token_count = -1;
  Token::Token next_token; 
  while (program_to_compile >> next_token) {
    if (std::holds_alternative<Token::UnknownToken>(next_token)) {
      unknown_token_count++;
    }
    scanned_tokens.emplace_back(next_token);
  }

  std::cout << "unknown_token_count = " << unknown_token_count << "\n";
  if (unknown_token_count >= 1) {
    throw std::runtime_error("invalid program input. failed to compile");
  }

  // compiled successfully! keep doing what you do!

  return 0;
}
