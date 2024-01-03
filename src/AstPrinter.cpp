#include "AST.h"

#include <iostream>
#include <variant>

namespace bf {
class AstPrinter final {
public:
  void print(const Program &Program) { print(Program.body); }

  void print(const NodeList &statements) {
    for (auto &stmt : statements) {
      std::visit(*this, stmt);
    }
  }

  void operator()(const ZeroPtr &) const { std::cout << "[+]"; }

  void operator()(const ReadPtr &) const { std::cout << ","; }

  void operator()(const PrintPtr &) const { std::cout << "."; }

  void operator()(const LoopPtr &Loop) {
    std::cout << "[";
    print(Loop->body);
    std::cout << "]";
  }

  void operator()(const MovePtr &move) const {
    if (move->amount != 0) {
      std::cout << std::string(std::abs(move->amount), move->amount < 0 ? '<' : '>');
    }
  }

  void operator()(const AddPtr &add) const {
    if (add->amount) {
      std::cout << std::string(std::abs(add->amount), add->amount < 0 ? '-' : '+');
    }
  }
};
} // namespace bf
