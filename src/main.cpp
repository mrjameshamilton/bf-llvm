#include "AstPrinter.cpp"
#include "Compiler.cpp"
#include "Interpreter.cpp"
#include "Parser.cpp"
#include "Scanner.cpp"
#include "llvm/Support/CommandLine.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

cl::opt<std::string> InputFilename(cl::Positional, cl::desc("<input>"), cl::Required);
cl::opt<std::string> OutputFilename("o", cl::desc("Output LLVM IR file"), cl::value_desc("<output>"));
cl::opt<bool> Execute("e", cl::desc("Execute the brainf*ck program using the interpreter"));
cl::opt<bool> Print("p", cl::desc("Print the brainf*ck source"));
cl::opt<bool> DontOptimize("dontoptimize", cl::desc("Don't optimize the LLVM IR"));

std::string read_string_from_file(const std::string &file_path) {
  const std::ifstream input_stream(file_path, std::ios_base::binary);

  if (input_stream.fail()) {
    throw std::runtime_error("Failed to open file");
  }

  std::stringstream buffer;
  buffer << input_stream.rdbuf();

  return buffer.str();
}

int main(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv);

  if (InputFilename.empty()) {
    std::cout << "Source must not be empty";
    return -1;
  }

  bf::Scanner Scanner(read_string_from_file(InputFilename));
  std::vector<bf::Token> tokens = Scanner.scanTokens();
  bf::Parser Parser(tokens);
  try {
    auto AST = Parser.parse();

    if (Print.getValue()) {
      bf::AstPrinter AstPrinter;
      AST->accept(AstPrinter);
    }

    if (OutputFilename.empty() || Execute.getValue()) {
      bf::Interpreter Interpreter;
      AST->accept(Interpreter);
    }

    if (!OutputFilename.empty()) {
      bf::Compiler Compiler;
      AST->accept(Compiler);
      if (!DontOptimize.getValue()) {
        Compiler.optimize();
      }
      Compiler.writeIR(OutputFilename.getValue());
    }
  } catch (bf::ParseError &E) {
    std::cerr << "Line " << E.getLine() << ": " << E.what() << "\n";
    return -1;
  }
  return 0;
}
