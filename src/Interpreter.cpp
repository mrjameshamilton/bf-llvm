#include "AST.h"

namespace bf {
class Interpreter : public Visitor {
private:
  unsigned char Memory[30000] = {0};
  unsigned char *Ptr = Memory;

public:
  Interpreter() = default;

  void visitMove(const Move &Move) override { Ptr += Move.getAmount(); }

  void visitAdd(const Add &Add) override { *Ptr += Add.getAmount(); }

  void visitZero(const Zero &Zero) override { *Ptr = 0; }

  void visitLoop(const Loop &Loop) override {
    while (*Ptr)
      Loop.bodyAccept(*this);
  }

  void visitPrint(const Print &Print) override { putchar(*Ptr); }

  void visitRead(const Read &Read) override { *Ptr = getchar(); }

  void visitProgram(const Program &Program) override { Program.bodyAccept(*this); }
};
} // namespace bf
