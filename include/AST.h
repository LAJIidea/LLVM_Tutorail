//
// Created by BY210033 on 2022/11/2.
//

#ifndef LLVM_TUTORAIL_AST_H
#define LLVM_TUTORAIL_AST_H

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>


#include <string>
#include <memory>
#include <vector>

//===------------------------------------------===//
// Abstract Syntax Tree (aka Parser Tree)
//===------------------------------------------===//
namespace toy {

    /// ExprAST - Base class for all expression nodes.
    /// Note that instead of adding virtual methods to the ExprAST class hierarchy,
    /// it could also make sense to use a visitor pattern or some other way to model
    /// this. For our purpose, adding a virtual method is simplest.
    class ExprAST {
    public:
        enum ExprASTKind {
            Expr_Var,
            Expr_Num,
            Expr_BinOp,
            Expr_Call
        };
        virtual ~ExprAST() = default;
        virtual llvm::Value *codegen() = 0;
    };

    /// NumberExprAST - Expression class for numeric literals like "1.0".
    class NumberExprAST : public ExprAST {
        double Val;

    public:
        explicit NumberExprAST(double Val): Val(Val) {}
        llvm::Value *codegen() override;
    };

    /// VariableExprAST - Expression class for referencing a variable, like "a".
    class VariableExprAST : public ExprAST {
        std::string Name;

    public:
        explicit VariableExprAST(const std::string &Name): Name(Name) {}

        llvm::Value *codegen() override;
    };

    /// BinaryExprAST - Expression class for a binary operator.
    class BinaryExprAST : public ExprAST {
        char Op;
        std::unique_ptr<ExprAST> LHS, RHS;

    public:
        BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
                      std::unique_ptr<ExprAST> RHS): Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
        llvm::Value *codegen() override;
    };

    /// CallExprAST - Expression class for function calls.
    class CallExprAST : public ExprAST {
        std::string Callee;
        std::vector<std::unique_ptr<ExprAST>> Args;

    public:
        CallExprAST(const std::string& Callee, std::vector<std::unique_ptr<ExprAST>> Args)
            : Callee(Callee), Args(std::move(Args)) {}
        llvm::Value *codegen() override;
    };

    /// PrototypeAST - This class represents the "prototype" for a function,
    /// which captures its name, and its argument names (thus implicitly the number
    /// of a arguments the function takes).
    class PrototypeAST {
        std::string Name;
        std::vector<std::string> Args;

    public:
        PrototypeAST(const std::string &Name, std::vector<std::string> Args)
            : Name(Name), Args(std::move(Args)) {}

        const std::string &getName() const { return Name; }

        llvm::Function *codegen();
    };

    /// FunctionAST - This class represents a function definition itself.
    class FunctionAST {
        std::unique_ptr<PrototypeAST> Proto;
        std::unique_ptr<ExprAST> Body;

    public:
        FunctionAST(std::unique_ptr<PrototypeAST> Proto, std::unique_ptr<ExprAST> Body)
            : Proto(std::move(Proto)), Body(std::move(Body)) {}

        llvm::Function *codegen();
    };

    void InitializeModule();

    void showErrors();

} // End anonymous namespace

#endif //LLVM_TUTORAIL_AST_H
