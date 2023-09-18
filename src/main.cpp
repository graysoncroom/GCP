#include "scanning/token.h"
#include "parsing/parser.h"
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
  Token::Token next_token; 

  while (program_to_compile >> next_token) {
    if (std::holds_alternative<Token::UnknownToken>(next_token)) {
      // TODO: Deal with the fact that the program contains a token that cannot be scanned.
      throw std::runtime_error("Error in lexical analysis phase. Failed to scan.");
    }
    scanned_tokens.emplace_back(next_token);
  }

  //std::reverse(std::begin(scanned_tokens), std::end(scanned_tokens));

  for (auto& token : scanned_tokens) {
    std::cout << token << ' ';
    if (std::holds_alternative<Token::StatementEnd>(token)) {
      std::cout << std::endl;
    }
  }


  Parser::ASTNode *node = Parser::parse_ast(scanned_tokens);
  Parser::interpret_ast_llvm(node);
  //std::cout << Parser::interpret_ast(node) << std::endl;

  // compiled successfully! keep doing what you do!

  return 0;
}
