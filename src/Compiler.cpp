#include "AST.h"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Passes/PassBuilder.h>

using namespace llvm;
using namespace llvm::sys;

namespace bf {

class Compiler final {
  std::unique_ptr<LLVMContext> Context;
  std::unique_ptr<Module> BfModule;
  std::unique_ptr<IRBuilder<>> Builder;
  Function *MainFunction;
  Function *PutCharFunction;
  Function *GetCharFunction;
  Value *DP;
  Value *Mem;

  [[nodiscard]] Value *LoadByte() const { return Builder->CreateLoad(Builder->getInt8Ty(), DP); }

  void StoreByte(Value *Value) const { Builder->CreateStore(Value, DP); }

public:
  Compiler() : DP{}, Mem{} {
    Context = std::make_unique<LLVMContext>();
    BfModule = std::make_unique<Module>("bf", *Context);
    Builder = std::make_unique<IRBuilder<>>(*Context);
    const auto PutCharTy = FunctionType::get(Builder->getInt32Ty(), Builder->getInt32Ty(), false);
    PutCharFunction = Function::Create(PutCharTy, Function::ExternalLinkage, "putchar", *BfModule);
    const auto GetCharTy = FunctionType::get(Builder->getInt32Ty(), false);
    GetCharFunction = Function::Create(GetCharTy, Function::ExternalLinkage, "getchar", *BfModule);
    const auto FT = FunctionType::get(Builder->getInt32Ty(), false);
    MainFunction = Function::Create(FT, Function::ExternalLinkage, "main", *BfModule);
  }

  void compile(const Program &Program) {
    const auto EntryBasicBlock = BasicBlock::Create(*Context, "entry", MainFunction);
    Builder->SetInsertPoint(EntryBasicBlock);
    const auto ArrayType = ArrayType::get(Builder->getInt8Ty(), 30'000);
    const auto Memory = Builder->CreateAlloca(ArrayType, nullptr, "Memory");
    Memory->setAlignment(Align(8));
    Builder->CreateMemSet(Memory, Builder->getInt8(0), 30'000, Memory->getAlign());
    DP = Builder->CreateGEP(ArrayType, Memory, {Builder->getInt8(0), Builder->getInt8(0)}, "DP");
    for (const auto &stmt : Program.body) {
      std::visit(*this, stmt);
    }
    Builder->CreateRet(Builder->getInt32(0));
  }

  void operator()(const MovePtr &Move) {
    DP = Builder->CreateGEP(Builder->getInt8Ty(), DP, Builder->getInt8(Move->amount), "DP");
  }

  void operator()(const AddPtr &Add) const { StoreByte(Builder->CreateAdd(LoadByte(), Builder->getInt8(Add->amount))); }

  void operator()(const ZeroPtr &) const { StoreByte(Builder->getInt8(0)); }

  void operator()(const LoopPtr &Loop) {
    const auto Entry = Builder->GetInsertBlock();
    const auto Body = BasicBlock::Create(*Context, "Loop", MainFunction);
    const auto Exit = BasicBlock::Create(*Context, "Exit", MainFunction);

    Builder->CreateCondBr(Builder->CreateIsNotNull(LoadByte(), "EntryNotNull"), Body, Exit);

    Builder->SetInsertPoint(Exit);
    const auto DPExitPhi = Builder->CreatePHI(Builder->getPtrTy(), 2, "DPExitPhi");
    DPExitPhi->addIncoming(DP, Entry);

    Builder->SetInsertPoint(Body);
    const auto DPBodyPhi = Builder->CreatePHI(Builder->getPtrTy(), 2, "DPBodyPhi");
    DPBodyPhi->addIncoming(DP, Entry);

    // Set the current DP to the body DP.
    DP = DPBodyPhi;

    for (auto &stmt : Loop->body) {
      std::visit(*this, stmt);
    }

    // Body and exit DP values can come from the current DP.
    DPBodyPhi->addIncoming(DP, Builder->GetInsertBlock());
    DPExitPhi->addIncoming(DP, Builder->GetInsertBlock());

    Builder->CreateCondBr(Builder->CreateIsNotNull(LoadByte(), "ExitNotNull"), Body, Exit);

    DP = DPExitPhi;
    Builder->SetInsertPoint(Exit);
  }

  void operator()(const PrintPtr &) const {
    const auto Byte = LoadByte();
    const auto ExtendedByte = Builder->CreateSExt(Byte, Builder->getInt32Ty());
    Builder->CreateCall(PutCharFunction, ExtendedByte);
  }

  void operator()(const ReadPtr &) const {
    const auto Int32Value = Builder->CreateCall(GetCharFunction);
    const auto Byte = Builder->CreateTrunc(Int32Value, Builder->getInt8Ty());
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

  [[nodiscard]] bool writeIR(const std::string &Filename) const {
    std::error_code ec;
    auto out = raw_fd_ostream(Filename, ec);
    BfModule->print(out, nullptr);
    out.close();
    return ec.value() == 0;
  }
};
} // namespace bf
