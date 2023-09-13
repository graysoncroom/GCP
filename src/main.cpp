#include "scanning/token.h"
#include "utils/arguments.h"

#include <string>
#include <fstream>
#include <iostream>

namespace Parser {
  struct ASTNode {
    Token::Token token;
    ASTNode* left_child;
    ASTNode* right_child;
  };

  int interpret_ast(ASTNode *node) {
    if (node->left_child == nullptr || node->right_child == nullptr) {
      if (!Token::is_terminal(node->token)) { throw "AST Error"; }

      return std::get<Token::IntegerLiteral>(node->token).value;
    }

    int result = Token::call_operator_of(node->token, interpret_ast(node->left_child), interpret_ast(node->right_child));

    return result;
  }


  ASTNode *parse_ast(std::vector<Token::Token> scanned_tokens) {
    ASTNode *node = new ASTNode();

    size_t token_count = scanned_tokens.size();

    switch (token_count) {
      case 0:
        throw "Input File Empty. Invalid program.";
      case 1: {
        node->token = scanned_tokens[0];
        return node;
      }
    }
    
    
    //node->left_child = new ASTNode();
    //node->left_child->token = scanned_tokens[0];
    //node->token = scanned_token[1];

    //ASTNode *tmp_node = node;

    //size_t i = 2;
    //while (i < token_count) {
    //  tmp_node->left_child = new ASTNode();
    //  tmp_node->left_child->token = scanned_tokens[i];
    //  tmp_node->token = scanned_token[i+1];

    //  i += 2;
    //}
    
    node->left_child = new ASTNode();
    node->left_child->token = scanned_tokens[0];
    node->token = scanned_tokens[1];

    auto&& slice = std::vector<Token::Token>(
      scanned_tokens.begin() + 2, 
      scanned_tokens.end()
    );

    node->right_child = parse_ast(slice);
    
    return node;
    //auto& current_token = scanned_tokens[0];

    //if (Token::token_is_terminal(current_token)) {
    //  
    //}
    //return node;
  }
}

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

  for (auto &token : scanned_tokens) {
    std::cout << token << ' ';
  }
  std::cout << std::endl;


  Parser::ASTNode *node = Parser::parse_ast(scanned_tokens);
  std::cout << Parser::interpret_ast(node) << std::endl;

  // compiled successfully! keep doing what you do!

  return 0;
}
