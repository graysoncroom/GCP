#pragma once

#include <cctype>
#include <iostream>
#include <optional>
#include <variant>
#include <stdexcept>
#include <string_view>
#include <string>
//#include <boost/bimap.hpp>

namespace Token {
  // ====== Token Definitions ======
  struct UnknownToken {};

  // operators
  struct Plus  {};
  struct Minus {};
  struct Star  {};
  struct Slash {};

  // literals
  struct IntegerLiteral { int value; };

  using Token = std::variant<UnknownToken, Plus, Minus, Star, Slash, IntegerLiteral>;

  // ====== Token Helpers / Utilities ======
  template<class... Ts>
  struct overloaded : Ts... { using Ts::operator()...; };

  template<class... Ts>
  overloaded(Ts...) -> overloaded<Ts...>;

  template <typename T>
  T call_operator_of(const Token& token, T x, T y) {
    T result{};
    std::visit(overloaded {
      [&](const Plus&)            { result = x + y; },
      [&](const Minus&)           { result = x - y; },
      [&](const Star&)            { result = x * y; },
      [&](const Slash&)           { result = x / y; },
      [&](const IntegerLiteral&)  { throw std::runtime_error("[Token::call_operator_of] IntegerLiteral is not an operator"); },
      [&](const auto&)            { throw std::runtime_error("[Token::call_operator_of] Undefined token in visitor overload"); }
    }, token);
    return result;
  }

  bool is_terminal(const Token& token) {
    bool token_is_terminal;
    std::visit(overloaded {
      [&](const Plus&)            { token_is_terminal = false; },
      [&](const Minus&)           { token_is_terminal = false; },
      [&](const Star&)            { token_is_terminal = false; },
      [&](const Slash&)           { token_is_terminal = false; },
      [&](const IntegerLiteral &) { token_is_terminal = true; },
      [&](const auto&)            { throw std::runtime_error("[Token::is_terminal] Undefined token in visitor overload"); }
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

  std::ostream& operator<<(std::ostream& os, const Token& token) {
    std::visit(overloaded {
      [&os](const Plus&)             { os << "+"; },
      [&os](const Minus&)            { os << "-"; },
      [&os](const Star&)             { os << "*"; },
      [&os](const Slash&)            { os << "/"; },
      [&os](const IntegerLiteral &t) { os << t.value; },
      [&os](const auto&)             { throw std::runtime_error("[Token::operator<<] Undefined token in visitor overload"); }
    }, token);

    return os;
  }

  std::istream& operator>>(std::istream& is, Token& token) {
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
