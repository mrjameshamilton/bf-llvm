#include "AST.h"
#include "Token.h"
#include <initializer_list>
#include <iostream>
#include <memory>
#include <numeric>
#include <vector>

namespace bf {

class ParseError final : public std::runtime_error {
  unsigned int line = 0;

public:
  explicit ParseError(const std::string &message, const unsigned int line) : runtime_error(message), line{line} {}

  [[nodiscard]] unsigned int getLine() const { return line; }
};

class Parser {
public:
  explicit Parser(const std::vector<Token> &tokens) : tokens{tokens} {}

  auto parse() {
    auto statements = NodeList();
    while (!isAtEnd()) {
      instruction(statements);
    }
    return std::make_unique<Program>(statements);
  }

private:
  unsigned int current = 0;
  std::vector<Token> tokens;

  static ParseError error(const Token token, const std::string &message) {
    return ParseError(message, token.getLine());
  }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
  void instruction(NodeList &body) {
    if (isAtStart()) {
      body.push_back(std::move(_instruction()));
      return;
    }

    // Consecutive Add or Move are combined.
    const auto PreviousType = previous().getType();
    const auto CurrentType = peek().getType();

    if (PreviousType == TokenType::PLUS && CurrentType == TokenType::PLUS ||
        PreviousType == TokenType::MINUS && CurrentType == TokenType::MINUS) {
      dynamic_cast<Add *>(body.back().get())->amount += dynamic_cast<Add *>(_instruction().get())->amount;
    } else if (PreviousType == TokenType::GREATER && CurrentType == TokenType::GREATER ||
               PreviousType == TokenType::LESS && CurrentType == TokenType::LESS) {
      dynamic_cast<Move *>(body.back().get())->amount += dynamic_cast<Move *>(_instruction().get())->amount;
    } else {
      body.push_back(std::move(_instruction()));
    }
  }

  std::unique_ptr<Node> _instruction() {
    if (match(TokenType::LEFT_BRACKET)) {
      auto body = NodeList();
      while (!check(TokenType::RIGHT_BRACKET) && !isAtEnd()) {
        instruction(body);
      }
      consume(TokenType::RIGHT_BRACKET, std::string("Expected matching ']'."));
      // [+] or [-] zero out the current Memory cell, so can be
      // replaced by a specific zero-ing _instruction.
      if (body.size() == 1 && dynamic_cast<const Add *>(body.at(0).get())) {
        return std::make_unique<Zero>();
      } else {
        return std::make_unique<Loop>(body);
      }
    } else if (match(TokenType::RIGHT_BRACKET))
      throw error(previous(), std::string("Unexpected ']'."));
    else if (match(TokenType::GREATER))
      return std::make_unique<Move>(1);
    else if (match(TokenType::LESS))
      return std::make_unique<Move>(-1);
    else if (match(TokenType::PLUS))
      return std::make_unique<Add>(1);
    else if (match(TokenType::MINUS))
      return std::make_unique<Add>(-1);
    else if (match(TokenType::DOT))
      return std::make_unique<Print>();
    else if (match(TokenType::COMMA))
      return std::make_unique<Read>();

    throw error(peek(), std::string("Expected bf command."));
  }
#pragma clang diagnostic pop

  Token consume(const TokenType type, const std::string &message) {
    if (check(type))
      return advance();

    throw error(peek(), message);
  }

  template <typename T> bool match(const std::initializer_list<T> &types) {
    for (auto &type : types) {
      if (check(type)) {
        advance();
        return true;
      }
    }
    return false;
  }

  bool match(const TokenType type) { return match({type}); }

  [[nodiscard]] bool check(const TokenType type) const { return !isAtEnd() && peek().getType() == type; }

  Token advance() {
    if (!isAtEnd())
      current++;
    return previous();
  }

  [[nodiscard]] bool isAtStart() const { return current == 0; }

  [[nodiscard]] bool isAtEnd() const { return peek().getType() == TokenType::END; }

  [[nodiscard]] Token peek() const { return tokens.at(current); }

  [[nodiscard]] Token previous() const { return tokens.at(current - 1); }
};
} // namespace bf
