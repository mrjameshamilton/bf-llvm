#ifndef COMPILER_H
#define COMPILER_H

#include "AST.h"
#include <llvm/IR/BasicBlock.h>
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
        Value *DP;

        [[nodiscard]] Value *LoadByte() const { return Builder->CreateLoad(Builder->getInt8Ty(), DP); }

        void StoreByte(Value *Value) const { Builder->CreateStore(Value, DP); }

    public:
        Compiler() : DP{} {
            Context = std::make_unique<LLVMContext>();
            BfModule = std::make_unique<Module>("bf", *Context);
            Builder = std::make_unique<IRBuilder<>>(*Context);
            MainFunction = Function::Create(
                FunctionType::get(Builder->getInt32Ty(), false),
                Function::ExternalLinkage,
                "main",
                *BfModule
            );
        }

        void compile(const Program &Program);
        void operator()(const MovePtr &Move);
        void operator()(const AddPtr &Add) const;
        void operator()(const ZeroPtr &) const;
        void operator()(const LoopPtr &Loop);
        void operator()(const PrintPtr &) const;
        void operator()(const ReadPtr &) const;
        void optimize() const;
        [[nodiscard]] bool writeIR(const std::string &Filename) const;
    };
}// namespace bf
#endif//COMPILER_H
