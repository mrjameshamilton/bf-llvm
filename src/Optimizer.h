#ifndef OPTIMIZER_H
#define OPTIMIZER_H
#include "AST.h"
#include "Util.h"

#include <ranges>

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

                // Visit loop bodies to recursively shrink.
                if (std::holds_alternative<LoopPtr>(node)) {
                    const auto loop = std::get<LoopPtr>(node);
                    loop->body = shrink(loop->body);
                }
            }

            return to<std::vector<Node>>(
                result | std::views::filter([](auto &n) {
                    return std::visit(overloaded{
                                          [](const AddPtr &Add) { return Add->amount != 0; },
                                          [](const MovePtr &Move) { return Move->amount != 0; },
                                          [](const auto &) { return true; },
                                      },
                                      n);
                })
            );
        }
    };
}// namespace bf


#endif//OPTIMIZER_H
