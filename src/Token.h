#ifndef BF_TOKEN_H
#define BF_TOKEN_H

namespace bf {
enum class TokenType { LEFT_BRACKET, RIGHT_BRACKET, PLUS, MINUS, COMMA, DOT, LESS, GREATER, END };

class Token {
  TokenType type;
  unsigned int line;

public:
  explicit Token(const TokenType type, const unsigned int line) : type{type}, line{line} {}

  [[nodiscard]] TokenType getType() const { return type; }

  [[nodiscard]] unsigned int getLine() const { return line; }
};
} // namespace bf

#endif // BF_TOKEN_H
