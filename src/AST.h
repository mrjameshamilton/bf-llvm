#ifndef BF_AST_H
#define BF_AST_H

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace bf {
class Move;

class Add;

class Zero;

class Loop;

class Print;

class Read;

class Program;

class Visitor {
public:
  virtual void visitMove(const Move &Move) = 0;

  virtual void visitAdd(const Add &Add) = 0;

  virtual void visitZero(const Zero &Zero) = 0;

  virtual void visitLoop(const Loop &Loop) = 0;

  virtual void visitPrint(const Print &Print) = 0;

  virtual void visitRead(const Read &Read) = 0;

  virtual void visitProgram(const Program &Program) = 0;
};

class Node {
public:
  virtual void accept(Visitor &visitor) const = 0;
  virtual ~Node() = default;
};

using NodeList = std::vector<std::unique_ptr<Node>>;

class Program : public Node {
private:
  NodeList body;

public:
  explicit Program(NodeList &body) : body{std::move(body)} {}

  void accept(Visitor &visitor) const override { visitor.visitProgram(*this); }

  void bodyAccept(Visitor &visitor) const {
    for (auto &N : body) {
      N->accept(visitor);
    }
  }
};

class Parser;

class Move : public Node {
  // Parser is a friend so the amount can be modified.
  friend Parser;

private:
  int amount = 0;

public:
  explicit Move(int move) : amount{move} {}

  [[nodiscard]] int getAmount() const { return amount; }

  void accept(Visitor &visitor) const override { visitor.visitMove(*this); }
};

class Add : public Node {
  // Parser is a friend so the amount can be modified.
  friend Parser;

private:
  int amount = 0;

public:
  explicit Add(int amount) : amount{amount} {}

  [[nodiscard]] int getAmount() const { return amount; }

  void accept(Visitor &visitor) const override { visitor.visitAdd(*this); }
};

class Zero : public Node {
public:
  explicit Zero() = default;

  void accept(Visitor &visitor) const override { visitor.visitZero(*this); }
};

class Loop : public Node {
private:
  NodeList body;

public:
  explicit Loop(NodeList &body) : body{std::move(body)} {}

  void accept(Visitor &visitor) const override { visitor.visitLoop(*this); }

  void bodyAccept(Visitor &visitor) const {
    for (auto &n : body) {
      n->accept(visitor);
    }
  }
};

class Print : public Node {
public:
  explicit Print() = default;

  void accept(Visitor &visitor) const override { visitor.visitPrint(*this); }
};

class Read : public Node {
public:
  explicit Read() = default;

  void accept(Visitor &visitor) const override { visitor.visitRead(*this); }
};

} // namespace bf
#endif // BF_AST_H
