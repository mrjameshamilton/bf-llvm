#include "Token.h"
#include <string>
#include <utility>
#include <vector>

namespace bf {
class Scanner {
public:
  explicit Scanner(std::string Source) : Source{std::move(Source)} {}

  std::vector<Token> scanTokens() {
    while (!isAtEnd()) {
      scanToken();
    }

    tokens.emplace_back(TokenType::END, line);

    return tokens;
  }

private:
  const std::string Source;
  signed int current = 0;
  signed int line = 1;
  std::vector<Token> tokens = std::vector<Token>();

  void addToken(TokenType type) { tokens.emplace_back(type, line); }

  char advance() { return Source.at(current++); }

  [[nodiscard]] bool isAtEnd() const { return current >= Source.length(); }

  void scanToken() {
    switch (advance()) {
      case '[': addToken(TokenType::LEFT_BRACKET); break;
      case ']': addToken(TokenType::RIGHT_BRACKET); break;
      case ',': addToken(TokenType::COMMA); break;
      case '.': addToken(TokenType::DOT); break;
      case '-': addToken(TokenType::MINUS); break;
      case '+': addToken(TokenType::PLUS); break;
      case '<': addToken(TokenType::LESS); break;
      case '>': addToken(TokenType::GREATER); break;
      case '\n': line++; break;
      default:
        {
          // Ignore other characters.
        }
    }
  }
};
} // namespace bf
