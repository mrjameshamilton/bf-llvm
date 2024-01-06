#ifndef BF_AST_H
#define BF_AST_H

#include "AST.h"

#include <memory>
#include <variant>
#include <vector>

namespace bf {

    struct Move;
    struct Add;
    struct Loop;
    struct Zero final {};
    struct Print final {};
    struct Read final {};

    using AddPtr = std::unique_ptr<Add>;
    using MovePtr = std::unique_ptr<Move>;
    using ZeroPtr = std::unique_ptr<Zero>;
    using LoopPtr = std::unique_ptr<Loop>;
    using PrintPtr = std::unique_ptr<Print>;
    using ReadPtr = std::unique_ptr<Read>;

    using Node = std::variant<AddPtr, MovePtr, ZeroPtr, LoopPtr, PrintPtr, ReadPtr>;

    using NodeList = std::vector<Node>;

    struct Program final {
        NodeList body;

        explicit Program(NodeList &body) : body{std::move(body)} {}
    };

    struct Move final {
        int amount = 0;
    };

    struct Add final {
        int amount = 0;
    };

    struct Loop final {
        NodeList body;

        explicit Loop(NodeList &body) : body{std::move(body)} {}
    };

}// namespace bf
#endif// BF_AST_H
