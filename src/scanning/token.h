#pragma once

#include <cctype>
#include <iostream>
#include <optional>
#include <type_traits>
#include <variant>
#include <stdexcept>
#include <string_view>
#include <string>
//#include <boost/bimap.hpp>

namespace Token {
  // ====== Token Definitions ======
  struct UnknownToken {};
  struct StatementEnd {};

  // binary operators
  struct Plus  {};
  struct Minus {};
  struct Star  {};
  struct Slash {};
  struct Assignment {};

  // unary operators
  struct Print {};

  // variable-related stuff
  struct TypeInteger {};
  struct TypeFloat   {}; // TODO: Unimplemented
  struct TypeDouble  {}; // TODO: Unimplemented

  struct GlobalVariable    { std::string identifier; };
  struct MutableVariable   { std::string identifier; }; // TODO: Unimplemented
  struct ImmutableVariable { std::string identifier; }; // TODO: Unimplemented

  // literals
  struct IntegerLiteral { int value; };

  using Token = std::variant<UnknownToken, StatementEnd, Plus, Minus, Star, Slash, Assignment, Print, TypeInteger, IntegerLiteral, GlobalVariable>;

  // ====== Token Helpers / Utilities ======
  template<class... Ts>
  struct overloaded : Ts... { using Ts::operator()...; };

  template<class... Ts>
  overloaded(Ts...) -> overloaded<Ts...>;

  // Use implicit type conversions that C++ uses for + operator for all operators when
  // running this code interactively. Going to deprecate this interactive feature soon.
  template <typename T, typename U>
  auto call_operator_of(const Token& token, T& x, U& y) -> typename std::decay_t<decltype(x + y)> {
    typename std::decay_t<decltype(x+y)> result{};
    std::visit(overloaded {
      [&](const Plus&)           { result = x + y; },
      [&](const Minus&)          { result = x - y; },
      [&](const Star&)           { result = x * y; },
      [&](const Slash&)          { result = x / y; },
      [&](const Assignment&)     { x = y; result = x; },
      [&](const GlobalVariable&) { throw std::runtime_error("[Token::call_operator_of] GlobalVariable is not an operator"); },
      [&](const TypeInteger&)    { throw std::runtime_error("[Token::call_operator_of] TypeInteger is not an operator"); },
      [&](const IntegerLiteral&) { throw std::runtime_error("[Token::call_operator_of] IntegerLiteral is not an operator"); },
      [&](const auto&)           { throw std::runtime_error("[Token::call_operator_of] Undefined token in visitor overload"); }
    }, token);
    return result;
  }

  bool is_terminal(const Token& token) {
    bool token_is_terminal;
    std::visit(overloaded {
      [&](const Plus&)           { token_is_terminal = false; },
      [&](const Minus&)          { token_is_terminal = false; },
      [&](const Star&)           { token_is_terminal = false; },
      [&](const Slash&)          { token_is_terminal = false; },
      [&](const Print&)          { token_is_terminal = false; },
      [&](const Assignment&)     { token_is_terminal = false; },
      [&](const GlobalVariable&) { token_is_terminal = false; },
      [&](const TypeInteger&)    { token_is_terminal = false; },
      [&](const IntegerLiteral&) { token_is_terminal = true; },
      [&](const auto&)           { throw std::runtime_error("[Token::is_terminal] Undefined token in visitor overload"); }
    }, token);

    return token_is_terminal;
  }
  
  std::optional<Token> construct_token(const std::string_view& str) {
    if      (str == "+")     { return Plus{}; }
    else if (str == "-")     { return Minus{}; }
    else if (str == "*")     { return Star{}; }
    else if (str == "/")     { return Slash{}; }
    else if (str == ";")     { return StatementEnd{}; }
    else if (str == "print") { return Print{}; }
    else if (str == "int")   { return TypeInteger{}; }
    else if (str == "=")     { return Assignment{}; }
    else                     { return std::nullopt; }
  }

  bool is_well_formed_identifier(const std::string_view&) {
    return true; // TODO: Only return true if it is a valid c-style identifier
  }

  std::ostream& operator<<(std::ostream& os, const Token& token) {
    std::visit(overloaded {
      [&os](const Plus&)             { os << "+"; },
      [&os](const Minus&)            { os << "-"; },
      [&os](const Star&)             { os << "*"; },
      [&os](const Slash&)            { os << "/"; },
      [&os](const Print&)            { os << "print"; },
      [&os](const Assignment&)       { os << "="; },
      [&os](const StatementEnd&)     { os << ";"; },
      [&os](const TypeInteger&)      { os << "int"; },
      [&os](const GlobalVariable& t) { os << t.identifier; },
      [&os](const IntegerLiteral& t) { os << t.value; },
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
        std::string expr;
        auto expr_has_been_identified = false;
        while (!std::isspace(c)) {
          expr += c;
          is.get(c);

          std::optional<Token> maybe_expr_token = construct_token(expr);
          if (maybe_expr_token.has_value()) {
            token = *std::move(maybe_expr_token);
            expr_has_been_identified = true;
            break;
          }
        }

        if (!expr_has_been_identified) {
          if (is_well_formed_identifier(expr)) {
            token = GlobalVariable{expr};
          }
          else {
            token = UnknownToken{};
            is.setstate(std::ios_base::failbit);

            // fail to read, put expr back on istream
            for (size_t i = expr.size(); i > 0; --i) { is.unget(); }
          }
        }
      }
    }

    return is;
  }
}
