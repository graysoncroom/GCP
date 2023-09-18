#pragma once

#include "generator/generator.h"

#include <stdexcept>
#include <vector>
#include <variant>
#include <optional>

namespace Parser {
  struct ASTNode {
    Token::Token token;
    std::vector<ASTNode*> children;
  };

  // TODO: Right now, interpret_ast returns an int. This is fine if the only values are ints
  // and if everything evalutates to some value. Otherwise it needs to return something else.
  //template <typename T>
  //std::optional<T> interpret_ast(ASTNode *node) {
  //  if (node->left_child != nullptr && node->right_child == nullptr) { /* deal with unary operators */ }
  //  else if (node->left_child == nullptr && node->right_child == nullptr) {
  //    if (!Token::is_terminal(node->token)) { throw std::runtime_error("[Parser::interpret_ast] Operator has no children"); }

  //    return std::get<Token::IntegerLiteral>(node->token).value;
  //  }

  //  int result = Token::call_operator_of(node->token, interpret_ast(node->left_child), interpret_ast(node->right_child));

  //  return result;
  //}

  void interpret_ast_llvm_helper(ASTNode *node) {
    if (node->children.size() == 1) {
      // Deal with Unary operators like print
      interpret_ast_llvm_helper(node->children[0]);
      Generator::generate_llvm(node->token);
      return;
    }
    if (node->children.size() == 0) {
      if (!Token::is_terminal(node->token)) { throw std::runtime_error("[Parser::interpret_ast_llvm_helper] Operator has no children"); }

      Generator::generate_llvm(node->token);
      return;
    }
    
    //interpret_ast_llvm_helper(node->children[0]);
    //interpret_ast_llvm_helper(node->children[1]);
    for (auto *node : node->children) {
      interpret_ast_llvm_helper(node);
    }
    Generator::generate_llvm(node->token);
  }

  void interpret_ast_llvm(ASTNode *node) {
    Generator::generate_llvm_initialize();
    interpret_ast_llvm_helper(node);
    Generator::generate_llvm_finalize();
  }

  ASTNode *parse_ast_helper(std::vector<Token::Token> scanned_tokens) {
    ASTNode *node = new ASTNode();

    size_t token_count = scanned_tokens.size();

    switch (token_count) {
      case 0:
        throw std::runtime_error("[Parser::parse_ast_helper] Scanned input file has no tokens (Invalid program)");
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

    if (std::holds_alternative<Token::Print>(scanned_tokens[0])) {
      node->token = scanned_tokens[0];

      auto&& slice = std::vector<Token::Token>(
        scanned_tokens.begin() + 1, 
        scanned_tokens.end()
      );

      node->children.push_back(parse_ast_helper(slice));
    }
    else {
      node->token = scanned_tokens[1];

      node->children.push_back(new ASTNode());
      node->children[0]->token = scanned_tokens[0];

      auto&& slice = std::vector<Token::Token>(
        scanned_tokens.begin() + 2, 
        scanned_tokens.end()
      );

      node->children.push_back(parse_ast_helper(slice));
    }

    return node;
    
    //auto& current_token = scanned_tokens[0];

    //if (Token::token_is_terminal(current_token)) {
    //  
    //}
    //return node;
  }

  ASTNode *parse_ast(std::vector<Token::Token> scanned_tokens) {
    std::vector<size_t> statement_end_idxs;
    for (size_t i = 0; i < scanned_tokens.size(); i++) {
      if (std::holds_alternative<Token::StatementEnd>(scanned_tokens[i])) {
        statement_end_idxs.push_back(i);
      }
    }
    ASTNode *root_node = new ASTNode();
    root_node->token = Token::UnknownToken{};

    int prev_idx = -1;
    for (auto& idx: statement_end_idxs) {
      auto&& slice = std::vector<Token::Token>(
        scanned_tokens.begin() + prev_idx + 1,
        scanned_tokens.begin() + idx
      );
      prev_idx = idx;
      root_node->children.push_back(parse_ast_helper(slice));
    }
    //auto&& final_statement_slice = std::vector<Token::Token>(
    //  scanned_tokens.begin() + prev_idx + 1,
    //  scanned_tokens.end()
    //);
    //root_node->children.push_back(parse_ast_helper(final_statement_slice));

    return root_node;
  }
}
