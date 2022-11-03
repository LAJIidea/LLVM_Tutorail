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

## Parser
Here you will learn:
- [x] Lexer
- [x] Parser
- [x] CodeGen
- [x] LLVM
- [X] LLVM IR

The operations support by the compiler are:
- [x] Loop ...


## Optimizer
LLVM IR通过IRBuilder构建，因此构建起本身会检查您调用它时是否存在持续折叠的机会。
并且它可以显著减少模型情况下生成的LLVM IR数量(这对具有宏预处理器或使用了很多常量的情况下很有效)。
如下：
```python
def test(x) 1 + 2 + x;
```
常量折叠后输出:
```asm
define double @test(double %x) {
entry:
        %addtmp = fadd double 3.0003+00, %x
        ret double %addtmp
}
```
无优化的输出：
```asm
define double @test(double %x) {
entry:
        %addtmp = fadd double 2.000e+00, 1.000e+00
        %addtmp1 = fadd double %addtmp, %x
        ret double %addtmp1
}
```

