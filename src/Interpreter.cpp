#include "AST.h"

namespace bf {

    class Interpreter final {
        unsigned char Memory[30000] = {0};
        unsigned char *Ptr = Memory;

    public:
        void interpret(const Program &Program) {
            for (const auto &stmt: Program.body) {
                std::visit(*this, stmt);
            }
        }

        void operator()(const MovePtr &Move) { Ptr += Move->amount; }

        void operator()(const AddPtr &Add) const { *Ptr += Add->amount; }

        void operator()(const ZeroPtr &) const { *Ptr = 0; }

        void operator()(const LoopPtr &Loop) {
            while (*Ptr) {
                for (const auto &stmt: Loop->body) {
                    std::visit(*this, stmt);
                }
            }
        }

        void operator()(const PrintPtr &) const { putchar(*Ptr); }

        void operator()(const ReadPtr &) const { *Ptr = getchar(); }
    };
}// namespace bf
