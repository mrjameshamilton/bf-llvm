#include "AST.h"
#include "Token.h"
#include <memory>
#include <variant>
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
            NodeList statements;
            while (!isAtEnd()) {
                statements.push_back(instruction());
            }
            return std::make_shared<Program>(statements);
        }

    private:
        unsigned int current = 0;
        std::vector<Token> tokens;

        static ParseError error(const Token token, const std::string &message) {
            return ParseError(message, token.getLine());
        }

        Node instruction() {
            if (match(TokenType::LEFT_BRACKET)) {
                NodeList body;
                while (!check(TokenType::RIGHT_BRACKET) && !isAtEnd()) {
                    body.push_back(instruction());
                }
                consume(TokenType::RIGHT_BRACKET, std::string("Expected matching ']'."));
                // [+] or [-] zero out the current Memory cell, so can be
                // replaced by a specific zero-ing _instruction.
                if (body.size() == 1 && std::holds_alternative<AddPtr>(body.at(0))) {
                    return std::make_shared<Zero>();
                }

                return std::make_shared<Loop>(body);
            }
            if (match(TokenType::RIGHT_BRACKET))
                throw error(previous(), std::string("Unexpected ']'."));
            if (match(TokenType::GREATER))
                return std::make_shared<Move>(1);
            if (match(TokenType::LESS))
                return std::make_shared<Move>(-1);
            if (match(TokenType::PLUS))
                return std::make_shared<Add>(1);
            if (match(TokenType::MINUS))
                return std::make_shared<Add>(-1);
            if (match(TokenType::DOT))
                return std::make_shared<Print>();
            if (match(TokenType::COMMA))
                return std::make_unique<Read>();

            throw error(peek(), std::string("Expected bf command."));
        }

        Token consume(const TokenType type, const std::string &message) {
            if (check(type))
                return advance();

            throw error(peek(), message);
        }

        bool match(const TokenType type) {
            if (check(type)) {
                advance();
                return true;
            }
            return false;
        }

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
}// namespace bf
