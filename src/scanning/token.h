#pragma once

#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <string>

#include <boost/bimap.hpp>
//#include <boost/assign.hpp>
//
// Token::Token

namespace Token {
  struct UnknownToken {};

  // operators
  struct Plus {
    template <typename T> T operator()(T x, T y) { return x + y; }
  };

  struct Minus {
    template <typename T> T operator()(T x, T y) { return x - y; }
  };

  struct Star {
    template <typename T> T operator()(T x, T y) { return x * y; }
  };

  struct Slash {
    template <typename T> T operator()(T x, T y) { return x / y; }
  };

  // literals
  struct IntegerLiteral { int value; };

  template<class... Ts>
  struct overloaded : Ts... { using Ts::operator()...; };

  template<class... Ts>
  overloaded(Ts...) -> overloaded<Ts...>;

  using Token = std::variant<UnknownToken, Plus, Minus, Star, Slash, IntegerLiteral>;

  template <typename T>
  T call_operator_of(Token &token, T x, T y) {
    T result{};
    std::visit(overloaded {
      [&](Plus&)            { result = x + y; },
      [&](Minus&)           { result = x - y; },
      [&](Star&)            { result = x * y; },
      [&](Slash&)           { result = x / y; },
      [&](IntegerLiteral &) { throw std::runtime_error("error"); },
      [&](auto&)            { throw std::runtime_error("error"); }
    }, token);
    return result;
  }

  bool is_terminal(Token &token) {
    bool token_is_terminal;
    std::visit(overloaded {
      [&](Plus&)            { token_is_terminal = false; },
      [&](Minus&)           { token_is_terminal = false; },
      [&](Star&)            { token_is_terminal = false; },
      [&](Slash&)           { token_is_terminal = false; },
      [&](IntegerLiteral &) { token_is_terminal = true; },
      [&](auto&)            { throw std::runtime_error("is terminal error"); }
    }, token);

    return token_is_terminal;
  }
  
  std::optional<Token> construct_token(const std::string_view &str) {
    if (str == "+") {
      return Plus{};
    }
    else if (str == "-") {
      return Minus{};
    }
    else if (str == "*") {
      return Star{};
    }
    else if (str == "/") {
      return Slash{};
    }
    else {
      return std::nullopt;
    }
  }

  std::ostream &operator<<(std::ostream &os, Token &token) {
    std::visit(overloaded {
      [&os](Plus&)             { os << "+"; },
      [&os](Minus&)            { os << "-"; },
      [&os](Star&)             { os << "*"; },
      [&os](Slash&)            { os << "/"; },
      [&os](IntegerLiteral &t) { os << t.value; },
      [&os](auto&)             { throw std::runtime_error("no string conversion available"); }
    }, token);

    return os;
  }

  std::istream &operator>>(std::istream &is, Token &token) {
    char c;
    is >> std::ws;  // Skip leading whitespace

    if (is.eof()) {
      token = UnknownToken{};
      is.setstate(std::ios_base::failbit);
      return is;
    }

    is.get(c);  // Read the next character

    if (isdigit(c)) {
      // Read an integer literal
      is.unget();  // Put the digit back to read the whole number
      int value;
      is >> value;
      token = IntegerLiteral{value};
    } 
    else {
      // Try to construct an operator token
      std::string symbol(1, c);
      std::optional<Token> opToken = construct_token(symbol);

      if (opToken.has_value()) {
        token = opToken.value();
      }
      else {
        token = UnknownToken{};
        is.setstate(std::ios_base::failbit);
      }
    }

    return is;
  }
}
