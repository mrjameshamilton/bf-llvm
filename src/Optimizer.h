#ifndef OPTIMIZER_H
#define OPTIMIZER_H
#include "AST.h"

template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

namespace bf {

    class Optimizer {

    public:
        void optimize(Program &program) {
            program.body = shrink(program.body);
        }

    private:
        // Merge consecutive Zero, Move, and Add nodes.
        NodeList shrink(const NodeList &original) {
            NodeList result;

            for (auto &node: original) {
                if (result.empty()) {
                    // first node, just add it
                    result.push_back(node);
                } else {
                    const auto &previous = result.back();

                    if (std::holds_alternative<ZeroPtr>(previous) && std::holds_alternative<ZeroPtr>(node)) {
                        // consecutive zero-ing instructions can be removed
                    } else if (std::holds_alternative<AddPtr>(previous) && std::holds_alternative<AddPtr>(node)) {
                        // merge consecutive Add instructions
                        std::get<AddPtr>(previous)->amount += std::get<AddPtr>(node)->amount;
                    } else if (std::holds_alternative<MovePtr>(previous) && std::holds_alternative<MovePtr>(node)) {
                        // merge consecutive Move instructions
                        std::get<MovePtr>(previous)->amount += std::get<MovePtr>(node)->amount;
                    } else {
                        result.push_back(node);
                    }
                }

                // Visit the loops to recursively shrink.
                std::visit(overloaded{[this](const LoopPtr &Loop) { Loop->body = shrink(Loop->body); }, [](auto &) {}}, node);
            }

            return result;
        }
    };
}// namespace bf


#endif//OPTIMIZER_H
