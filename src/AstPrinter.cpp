#include "AST.h"
#include "Util.h"

#include <iostream>
#include <variant>

namespace bf {
    class AstPrinter final {
    public:
        void print(const Program &Program) {
            for (auto &stmt: Program.body) {
                print(stmt);
            }
        }

    private:
        void print(const Node &Node) {
            std::visit(
                overloaded{
                    [](const AddPtr &Add) { std::cout << std::string(std::abs(Add->amount), Add->amount < 0 ? '-' : '+'); },
                    [](const MovePtr &Move) { std::cout << std::string(std::abs(Move->amount), Move->amount < 0 ? '<' : '>'); },
                    [](const ZeroPtr &) { std::cout << "[+]"; },
                    [](const ReadPtr &) { std::cout << ","; },
                    [](const PrintPtr &) { std::cout << "."; },
                    [this](const LoopPtr &Loop) {
                        std::cout << "[";
                        for (auto &stmt: Loop->body) {
                            print(stmt);
                        }
                        std::cout << "]";
                    },
                },
                Node
            );
        }
    };
}// namespace bf
