//
// Created by BY210033 on 2022/11/3.
//
#include "AST.h"

#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

using namespace llvm;
namespace toy {

    static std::unique_ptr<LLVMContext> TheContext;
    static std::unique_ptr<IRBuilder<>> Builder;
    static std::unique_ptr<Module> TheModule;
    static std::map<std::string, Value*> NameValues;

    Value *logErrorV(const char *str) {
        fprintf(stderr, "Error: %s\n", str);
        return nullptr;
    }

    Value *NumberExprAST::codegen() {
        // 在LLVM IR中，数字常量用ConstantFP类表示，该类在内部保存数值APFloat (APFloat具有保存任意精度浮点常量的能力).
        // 下面的就是创建一个ConstantFP,请注意，在LLVM IR中，常量值都是唯一的，并且是共享的。所以API使用foo::get()而不是new
        return ConstantFP::get(*TheContext, APFloat(Val));
    }

    Value *VariableExprAST::codegen() {
        // Look this variable up in the function.
        // 使用LLVM对变量的引用也非常简单。在简单版本中，我们假设变量已经在某处定义，其值可用。
        Value *v = NameValues[Name];
        if (!v)
            logErrorV("Unknown variable name");
        return v;
    }

    Value *BinaryExprAST::codegen() {
        // 这里基本思想是递归的创建左侧IR，然后是右侧。
        // 这里IRBuilder开始显示价值，它知道在哪里插入新创建的指令，只需要指定需要创建的指令和使用的操作数
        // 并可以选择性的为生成指令提供名称，LLVM的一个好处是每次只是一个提示，例如如果生成多个addtmp变量，
        // LLVM将自动为每个变量提供一个递增的，唯一的数字后缀。
        // LLVM指令收到严格规则约束：例如，加法指令的左右操作数必须具有相同的类型
        // LLVM指定fcmp指令总是返回一个i1值，这里我们通过uitofp指令将输入的无符号整形转换为浮点型
        Value *L = LHS->codegen();
        Value *R = RHS->codegen();
        if (!L || !R)
            return nullptr;

        switch (Op) {
            case '+':
                return Builder->CreateFAdd(L, R, "addtmp");
            case '-':
                return Builder->CreateFSub(L, R, "subtmp");
            case '*':
                return Builder->CreateFMul(L, R, "multmp");
            case '<':
                L = Builder->CreateFCmpULT(L, R, "cmptmp");
                return Builder->CreateUIToFP(L, Type::getDoubleTy(*TheContext), "booltmp");
            default:
                return logErrorV("invalid binary operator");
        }
    }

    Value *CallExprAST::codegen() {
        // 使用LLVM生成函数调用IR非常简单，通过在LLVM模块的符号表中进行函数名查找，
        // 一旦我们有了要调用的函数，我们就递归的对每个要传入的参数进行IR生成，最终创建调用指令集。
        // Look up the name in the global module table.
        Function *calleeF = TheModule->getFunction(Callee);
        if (!calleeF)
            return logErrorV("Unknown function referenced");

        // If argument mismatch error.
        if (calleeF->arg_size() != Args.size())
            return logErrorV("Incorrect # arguments passed");

        std::vector<Value *> ArgsV;
        for (unsigned i = 0, e = Args.size(); i != e; ++i) {
            ArgsV.push_back(Args[i]->codegen());
            if (!ArgsV.back())
                return nullptr;
        }

        return Builder->CreateCall(calleeF, ArgsV, "calltmp");
    }

    Function *PrototypeAST::codegen() {
        // 原型函数既用于代码体，也用于外部函数声明。
        // 这里返回的是一个Function，而不是一个具体的Value
        // 下面是定义参数类型，这里是double类型，并将这个参数类型传递给函数类型，函数返回类型也为double
        // 这里函数是非vararg(false参数指定)。在LLVM中类型和常量一样是唯一的，通过foo::get()获取。
        // 之后创新函数原型，指示了要使用的类型、链接和名称，以及插入那个模块。
        // 这里ExternalLinkage表示该函数可能在当前模块之后定义或者可调用模块外的功能
        // 最后给参数提供名称。此步骤是可选的，但是名称一致可以使IR更具有可读性，允许后续代码通过名称引用参数
        // Make the function type: double(double, double) etc.
        std::vector<Type*> Doubles(Args.size(), Type::getDoubleTy(*TheContext));
        FunctionType *FT = FunctionType::get(Type::getDoubleTy(*TheContext), Doubles, false);
        Function *F = Function::Create(FT, Function::ExternalLinkage, Name, TheModule.get());

        // Set names for all arguments.
        unsigned Idx = 0;
        for (auto &Arg : F->args())
            Arg.setName(Args[Idx++]);

        return F;
    }

    Function *FunctionAST::codegen() {
        // 完整函数定义需要附加函数体.
        // 首先在模块的符号表中搜索此函数的现有版本，以防止已使用外部声明
        // 此时无论何种情况，都希望函数体为空
        // First, check for an existing function from a previous 'extern' declaration.
        Function *TheFunction = TheModule->getFunction(Proto->getName());

        if (!TheFunction)
            TheFunction = Proto->codegen();

        if (!TheFunction)
            return nullptr;

        if (!TheFunction->empty())
            return (Function*) logErrorV("Function cannot be redefined.");

        // 开始进行函数块设置
        // 首先创建一个名为entry的新基本块，并告诉新基本块中指令集应插入的位置（末尾）
        // 将函数参数加入到map符号表中，以便后续节点访问.
        // Create a new basic block to start insertion into.
        BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", TheFunction);
        Builder->SetInsertPoint(BB);

        // Record the function arguments in the NamedValues map.
        NameValues.clear();
        for (auto &arg : TheFunction->args()) {
            NameValues[arg.getName().str()] = &arg;
        }

        if (Value *RetVal = Body->codegen()) {
            // Finish off the function.
            Builder->CreateRet(RetVal);

            // validate the generated code, checking for consistency.
            verifyFunction(*TheFunction);

            return TheFunction;
        }

        // Error reading body, remove function.
        TheFunction->eraseFromParent();
        return nullptr;
    }

    void InitializeModule() {
        // Open a new context and module.
        TheContext = std::make_unique<LLVMContext>();
        TheModule = std::make_unique<Module>("my cool jit", *TheContext);

        // Create a new builder for the module.
        Builder = std::make_unique<IRBuilder<>>(*TheContext);
    }

    void showErrors() {
        // Print out all the generated code.
        TheModule->print(errs(), nullptr);
    }

}