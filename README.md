# An optimizing LLVM Brainf*ck compiler

An optimizing [brainf*ck](http://brainfuck.org/brainfuck.html) compiler using LLVM.

Some optimizations are applied before code generation:

* Zero-ing loops (`[+]` / `[-]`) are represented as a single instruction
* Consecutive move and add instructions are merged into single instructions with
an amount parameter

Then LLVM optimizations are applied on the generated LLVM IR. As an example of these
powerful optimizations, brainf*ck HelloWorld will be optimized to simple `putchar` calls:

```llvm
// Input
>++++++++[<+++++++++>-]<.>++++[<+++++++>-]<+.+++++++..+++.
>>++++++[<+++++++>-]<++.------------.>++++++[<+++++++++>-]
<+.<.+++.------.--------.>>>++++[<++++++++>-]<+.

// Optimized IR output
define i32 @main() local_unnamed_addr #0 {
entry:
  %0 = tail call i32 @putchar(i32 72)
  %1 = tail call i32 @putchar(i32 101)
  %2 = tail call i32 @putchar(i32 108)
  %3 = tail call i32 @putchar(i32 108)
  %4 = tail call i32 @putchar(i32 111)
  %5 = tail call i32 @putchar(i32 44)
  %6 = tail call i32 @putchar(i32 32)
  %7 = tail call i32 @putchar(i32 87)
  %8 = tail call i32 @putchar(i32 111)
  %9 = tail call i32 @putchar(i32 114)
  %10 = tail call i32 @putchar(i32 108)
  %11 = tail call i32 @putchar(i32 100)
  %12 = tail call i32 @putchar(i32 33)
  ret i32 0
}
```

## Building and Executing

```shell
$ cmake -G Ninja -B build
$ ninja -C build
$ build/bf examples/helloworld.bf -o build/helloworld.ll
$ lli build/helloworld.ll
```

## Other targets

This project is a port of [bf](https://github.com/mrjameshamilton/bf) which is a brainf*ck compiler with backends for:

* JVM
* Smali
* Dex
* C
* LLVM IR
* ARM assembly
* WASM
* JavaScript
* Lox

# Useful brainf*ck resources

* [Brainf*ck language reference](http://brainfuck.org/brainfuck.html)
* [Sample programs by Daniel B Cristofani](http://brainfuck.org/)
* [Optimizing brainf*ck programs](http://calmerthanyouare.org/2015/01/07/optimizing-brainfuck.html)
* [Brainf*ck Wikipedia article](https://en.wikipedia.org/wiki/Brainfuck)
 
