# LLVM12 Tutorial

This tutorial is illustrated with a toy language called "KaleidoScope".Kaleidoscope is a procedural language that allows 
you to define functions, use conditionals, math, etc. Over the course of the tutorial, we'll extend Kaleidoscope to 
support the if/then/else construct, a for loop, user define operators, JIT compilation with a simple command line 
interface, debug info, etc.

For example, the following simple example computes [Fibonacci numbers](https://en.wikipedia.org/wiki/Fibonacci_number):
```python
# Compute the x'th fibonacci number.
def fib(x)
    if x < 3 then
        1
    else
        fib(x-1)+fib(x-2)

# This expression will compute the 40th number
fib(40)
```

Here you will learn:
- [x] Lexer
- [x] Parser
- [x] CodeGen
- [x] LLVM
- [X] LLVM IR
- [X] LLVM JIT

The operations support by the compiler are:
- [x] Loop
...