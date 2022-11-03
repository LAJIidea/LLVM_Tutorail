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

另一方面， IRBuilder它有如以下的限制：它在构建代码时将所有分析都与代码内联，如下所示：
```python
def test(x) (1+2+x)*(x+(1+2))
```

输出为：
```asm
define double @test(double %x) {
entry:
    %addtmp = fadd double 3.000e+00, %x
    %addtmp1 = fadd double %x, 3.0003+00
    %multmp = fmul double %addtmp, %addtmp1
    ret double %multmp
}
```
上面的例子中LHS和RHS值为相同的，可以优化成`tmp = x+3; result = tmp * tmp`,但不幸的是没有多少本地分析能够检测和纠正这一点。这需要两个转换：
表达式重新关联(使add的词法相同)和公共子表达式消除(CSE)以删除冗余的add指令。