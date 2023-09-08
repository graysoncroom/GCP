#pragma once

#include "scanner.h"

#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <string>

#include <boost/bimap.hpp>
//#include <boost/assign.hpp>

enum TokenType: int {
  UnknownToken,
  // operators
  Plus,
  Minus,
  Star,
  Slash,
  // literals
  IntegerLiteral
};

// Define a traits class to map TokenType to its corresponding value type 
template <TokenType T>
struct TokenTypeTraits {
  using ValueType = int; // TODO: change
}; 

template <>
struct TokenTypeTraits<TokenType::IntegerLiteral> {
  using ValueType = int;
};

/*
template <>
struct TokenTypeTraits<TokenType::StringLiteral> {
  using ValueType = std::string;
};
*/

template <TokenType T>
class Token {
public:
  using Value = TokenTypeTraits<T>::ValueType;

  using AnyToken = 
    std::variant<
      Token<TokenType::UnknownToken>,
      Token<TokenType::Plus>,
      Token<TokenType::Minus>,
      Token<TokenType::Star>,
      Token<TokenType::Slash>,
      Token<TokenType::IntegerLiteral>>;

  Token() = delete;
  constexpr Token(Value val = Value{}): value(val) {
    token_bimap =  
      { {TokenType::UnknownToken   , "unknown token"}
      , {TokenType::IntegerLiteral , "integer literal"}
      , {TokenType::Plus           , "+"}
      , {TokenType::Minus          , "-"} 
      , {TokenType::Star           , "*"} 
      , {TokenType::Slash          , "/"} 
      };
  }

  constexpr Value getValue() const { return value; }

  // Allow switch and comparisons.   
  constexpr operator TokenType() const 
  { return T; }

  // get the underlying int representation of TokenType enum `T`
  constexpr operator int() const
  { return static_cast<int>(T); }

  // Disallow `if (token) ...` shenanigans.
  explicit  operator bool() const = delete;

  constexpr bool operator==(Token a) const
  { return token_type == a.token_type && value == value; }

  constexpr bool operator!=(Token a) const
  { return token_type != a.token_type || value != value; }

  constexpr operator std::string() const
  { return Token::tttos(T); }

  // stott = string to token type
  constexpr friend TokenType stott(const std::string str)
  { return token_bimap.right.at(str); }
  
  // tttos = token type to string
  constexpr friend std::string tttos(const TokenType token_type)
  { return token_bimap.left.at(token_type); }


private:
  TokenType token_type;
  Value value;

  // Error: assign not constexpr
  /*
  using bimapType = boost::bimap<TokenType, std::string>;
  static constexpr bimapType tokenBimap = boost::assign::list_of<bimapType::relation>
    (TokenType::UnknownToken, "unknown token")
    (TokenType::IntegerLiteral, "integer literal")
    (TokenType::Plus, "+")
    (TokenType::Minus, "-")
    (TokenType::Star, "*")
    (TokenType::Slash, "/");
  */

  static boost::bimap<TokenType, std::string> token_bimap;
};

template<TokenType T>
std::ostream &operator<<(std::ostream &os, const Token<T> &a) {
  os << static_cast<std::string>(a);
  return os;
}
