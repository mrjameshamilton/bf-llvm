#include "AST.h"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Target/TargetIntrinsicInfo.h>
#include <llvm/Target/TargetOptions.h>

using namespace llvm;
using namespace llvm::sys;

namespace bf {
class Compiler : public Visitor {
private:
  std::unique_ptr<LLVMContext> Context;
  std::unique_ptr<Module> BfModule;
  std::unique_ptr<Function *> MainFunction;
  std::unique_ptr<IRBuilder<>> Builder;
  std::unique_ptr<Function *> PutCharFunction;
  std::unique_ptr<Function *> GetCharFunction;
  std::unique_ptr<Value *> DP;
  std::unique_ptr<Value *> Mem;

  inline Value *LoadByte() { return Builder->CreateLoad(Builder->getInt8Ty(), *DP); }

  inline void StoreByte(Value *Value) { Builder->CreateStore(Value, *DP); }

public:
  Compiler() {
    Context = std::make_unique<LLVMContext>();
    BfModule = std::make_unique<Module>("bf", *Context);
    Builder = std::make_unique<IRBuilder<>>(*Context);
    auto PutCharTy = FunctionType::get(Builder->getInt32Ty(), Builder->getInt32Ty(), false);
    PutCharFunction =
        std::make_unique<Function *>(Function::Create(PutCharTy, Function::ExternalLinkage, "putchar", BfModule.get()));
    auto GetCharTy = FunctionType::get(Builder->getInt32Ty(), false);
    GetCharFunction =
        std::make_unique<Function *>(Function::Create(GetCharTy, Function::ExternalLinkage, "getchar", BfModule.get()));
    auto FT = FunctionType::get(Builder->getInt32Ty(), false);
    MainFunction =
        std::make_unique<Function *>(Function::Create(FT, Function::ExternalLinkage, "main", BfModule.get()));
  }

  void visitProgram(const Program &Program) override {
    auto EntryBasicBlock = BasicBlock::Create(*Context, "entry", *MainFunction);
    Builder->SetInsertPoint(EntryBasicBlock);
    auto ArrayType = ArrayType::get(Builder->getInt8Ty(), 30'000);
    auto Memory = Builder->CreateAlloca(ArrayType, nullptr, "Memory");
    Memory->setAlignment(Align(8));
    Builder->CreateMemSet(Memory, Builder->getInt8(0), 30'000, Memory->getAlign());
    DP = std::make_unique<Value *>(
        Builder->CreateGEP(ArrayType, Memory, ArrayRef<Value *>{Builder->getInt8(0), Builder->getInt8(0)}, "DP"));
    Program.bodyAccept(*this);
    Builder->CreateRet(Builder->getInt32(0));
  }

  void visitMove(const Move &Move) override {
    DP = std::make_unique<Value *>(
        Builder->CreateGEP(Builder->getInt8Ty(), *DP, Builder->getInt8(Move.getAmount()), "DP"));
  }

  void visitAdd(const Add &Add) override {
    StoreByte(Builder->CreateAdd(LoadByte(), Builder->getInt8(Add.getAmount())));
  }

  void visitZero(const Zero &Zero) override { StoreByte(Builder->getInt8(0)); }

  void visitLoop(const Loop &Loop) override {
    auto Entry = Builder->GetInsertBlock();
    auto Body = BasicBlock::Create(*Context, "Loop", *MainFunction);
    auto Exit = BasicBlock::Create(*Context, "Exit", *MainFunction);

    Builder->CreateCondBr(Builder->CreateIsNotNull(LoadByte(), "EntryNotNull"), Body, Exit);

    Builder->SetInsertPoint(Exit);
    auto DPExitPhi = Builder->CreatePHI(Builder->getInt8PtrTy(), 2, "DPExitPhi");
    DPExitPhi->addIncoming(*DP, Entry);

    Builder->SetInsertPoint(Body);
    auto DPBodyPhi = Builder->CreatePHI(Builder->getInt8PtrTy(), 2, "DPBodyPhi");
    DPBodyPhi->addIncoming(*DP, Entry);

    // Set the current DP to the body DP.
    DP = std::make_unique<Value *>(DPBodyPhi);

    Loop.bodyAccept(*this);

    // Body and exit DP values can come from the current DP.
    DPBodyPhi->addIncoming(*DP, Builder->GetInsertBlock());
    DPExitPhi->addIncoming(*DP, Builder->GetInsertBlock());

    Builder->CreateCondBr(Builder->CreateIsNotNull(LoadByte(), "ExitNotNull"), Body, Exit);

    // Exit->moveAfter(Builder->GetInsertBlock());
    DP = std::make_unique<Value *>(DPExitPhi);
    Builder->SetInsertPoint(Exit);
  }

  void visitPrint(const Print &Print) override {
    auto Byte = LoadByte();
    auto ExtendedByte = Builder->CreateSExt(Byte, Builder->getInt32Ty());
    Builder->CreateCall(*PutCharFunction, ExtendedByte);
  }

  void visitRead(const Read &Read) override {
    auto Int32Value = Builder->CreateCall(*GetCharFunction);
    auto Byte = Builder->CreateTrunc(Int32Value, Builder->getInt8Ty());
    StoreByte(Byte);
  }

  void optimize() const {
    LoopAnalysisManager LAM;
    FunctionAnalysisManager FAM;
    CGSCCAnalysisManager CGAM;
    ModuleAnalysisManager MAM;
    PassBuilder PB;

    // PB.printPassNames(outs());
    PB.registerModuleAnalyses(MAM);
    PB.registerCGSCCAnalyses(CGAM);
    PB.registerFunctionAnalyses(FAM);
    PB.registerLoopAnalyses(LAM);
    PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

    ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(OptimizationLevel::O3);

    MPM.run(*BfModule, MAM);
  }

  bool writeIR(const std::string &Filename) {
    std::error_code ec;
    auto out = raw_fd_ostream(Filename, ec);
    BfModule->print(out, nullptr);
    out.close();
    return ec.value() == 0;
  }
};
} // namespace bf
