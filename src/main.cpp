#include "AstPrinter.h"
#include "Compiler.h"
#include "Interpreter.h"
#include "Optimizer.h"
#include "Parser.h"
#include "Scanner.h"
#include <iostream>
#include <llvm/Support/CommandLine.h>

cl::opt<std::string> InputFilename(cl::Positional, cl::desc("<input>"), cl::Required);
cl::opt<std::string> OutputFilename("o", cl::desc("Output LLVM IR file"), cl::value_desc("<output>"));
cl::opt<bool> Execute("e", cl::desc("Execute the brainf*ck program using the interpreter"));
cl::opt<bool> Print("p", cl::desc("Print the brainf*ck source"));
cl::opt<bool> DontOptimize("dontoptimize", cl::desc("Don't optimize the LLVM IR"));

int main(const int argc, char **argv) {
    cl::ParseCommandLineOptions(argc, argv);

    if (InputFilename.empty()) {
        std::cerr << "Source must not be empty" << std::endl;
        return -1;
    }

    bf::Scanner Scanner(read_string_from_file(InputFilename));
    const std::vector<bf::Token> tokens = Scanner.scanTokens();
    bf::Parser Parser(tokens);
    try {
        const auto AST = Parser.parse();

        bf::Optimizer optimizer;
        optimizer.optimize(*AST);

        if (Print.getValue()) {
            bf::AstPrinter AstPrinter;
            AstPrinter.print(*AST);
            std::cout << "" << std::endl;
        }

        if (OutputFilename.empty() || Execute.getValue()) {
            bf::Interpreter Interpreter;
            Interpreter.interpret(*AST);
        }

        if (!OutputFilename.empty()) {
            bf::Compiler Compiler;
            Compiler.compile(*AST);
            if (!DontOptimize.getValue()) {
                Compiler.optimize();
            }
            if (!Compiler.writeIR(OutputFilename.getValue())) {
                std::cerr << "Error writing LLVM IR" << std::endl;
                return -1;
            }
        }
    } catch (bf::ParseError &E) {
        std::cerr << "Line " << E.getLine() << ": " << E.what() << std::endl;
        return -1;
    }
    return 0;
}
