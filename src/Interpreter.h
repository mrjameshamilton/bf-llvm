#include "AST.h"
#include "Util.h"

namespace bf {

    class Interpreter final {
        unsigned char Memory[30000] = {0};
        unsigned char *Ptr = Memory;

    public:
        void interpret(const Program &Program) {
            for (auto &stmt: Program.body) {
                interpret(stmt);
            }
        }

    private:
        void interpret(Node Node) {
            std::visit(
                overloaded{
                    [this](const MovePtr &Move) { Ptr += Move->amount; },
                    [this](const AddPtr &Add) { *Ptr += Add->amount; },
                    [this](ZeroPtr &) { *Ptr = 0; },
                    [this](const LoopPtr &Loop) {
                        while (*Ptr) {
                            for (const auto &body: Loop->body) {
                                interpret(body);
                            }
                        }
                    },
                    [this](PrintPtr &) { putchar(*Ptr); },
                    [this](ReadPtr &) { *Ptr = getchar(); },
                },
                Node
            );
        }
    };
}// namespace bf
