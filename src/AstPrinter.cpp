#include "AST.h"

#include <iostream>

namespace bf {
class AstPrinter final : public Visitor {
public:
  void visitZero(const Zero &Zero) override { std::cout << "[+]"; }

  void visitRead(const Read &Read) override { std::cout << ","; }

  void visitPrint(const Print &Print) override { std::cout << "."; }

  void visitProgram(const Program &Program) override { Program.bodyAccept(*this); }

  void visitLoop(const Loop &Loop) override {
    std::cout << "[";
    Loop.bodyAccept(*this);
    std::cout << "]";
  }

  void visitMove(const Move &move) override {
    if (move.getAmount() != 0) {
      std::cout << std::string(std::abs(move.getAmount()), move.getAmount() < 0 ? '<' : '>');
    }
  }

  void visitAdd(const Add &add) override {
    if (add.getAmount()) {
      std::cout << std::string(std::abs(add.getAmount()), add.getAmount() < 0 ? '-' : '+');
    }
  }
};
} // namespace bf
