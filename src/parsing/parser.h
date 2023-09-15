#pragma once

#include "generator/generator.h"

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

  void interpret_ast_llvm_helper(ASTNode *node) {
    if (node->left_child == nullptr || node->right_child == nullptr) {
      if (!Token::is_terminal(node->token)) { throw "AST Error"; }

      Generator::generate_llvm(node->token);
      return;
    }
    
    interpret_ast_llvm_helper(node->left_child);
    interpret_ast_llvm_helper(node->right_child);
    Generator::generate_llvm(node->token);
  }

  void interpret_ast_llvm(ASTNode *node) {
    Generator::generate_llvm_initialize();
    interpret_ast_llvm_helper(node);
    Generator::generate_llvm_finalize();
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
