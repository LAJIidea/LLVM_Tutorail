//
// Created by BY210033 on 2022/11/1.
//
#include "Parse.h"

using namespace llvm;
namespace toy {

    void Parser::addBinOpPrecedence(char op, int level) {
        BinopPrecedence[op] = level;
    }

    int Parser::getTokPrecedence() {
        auto curTok = lexer.getCurToken();
        if (!isascii(curTok))
            return -1;

        // Make sure it's a declared binop.
        int TokPrec = BinopPrecedence[curTok];
        if (TokPrec <= 0)
            return -1;
        return TokPrec;
    }

    std::unique_ptr<ExprAST> Parser::logError(const char *str) {
        fprintf(stderr, "Error: %s\n", str);
        return nullptr;
    }

    std::unique_ptr<PrototypeAST> Parser::logErrorP(const char *str) {
        logError(str);
        return nullptr;
    }

    std::unique_ptr<ExprAST> Parser::parseExpression() {
        auto lhs = parsePrimary();
        if (!lhs)
            return nullptr;

        return parseBinOpRHs(0, std::move(lhs));
    }

    std::unique_ptr<ExprAST> Parser::parseNumberExpr() {
        auto result = std::make_unique<NumberExprAST>(lexer.getValue());
        lexer.getNextToken(); // consume the number
        return std::move(result);
    }

    std::unique_ptr<ExprAST> Parser::parseParenExpr() {
        lexer.getNextToken(); // eat '('.
        auto v = parseExpression();
        if (!v)
            return nullptr;

        if (lexer.getCurToken() != ')')
            return logError("expected ')'");
        lexer.getNextToken(); // eat ')'.
        return v;
    }

    std::unique_ptr<ExprAST> Parser::parseIdentifierExpr() {
        std::string idName = lexer.getId().str();

        lexer.getNextToken(); // eat identifier.

        if (lexer.getCurToken() != '(') // simple variable ref.
            return std::make_unique<VariableExprAST>(idName);

        // Call Expression
        lexer.getNextToken(); // eat '('.
        std::vector<std::unique_ptr<ExprAST>> args;
        if (lexer.getCurToken() != ')') {
            while (true) {
                // todo here position?
                if (auto arg = parseExpression())
                    args.push_back(std::move(arg));
                else
                    return nullptr;

                if (lexer.getCurToken() == ')')
                    break;

                if (lexer.getCurToken() != ',')
                    return logError("Expected ')' or ',' in argument list");
                lexer.getNextToken();
            }
        }

        // eat the ')'.
        lexer.getNextToken();

        return std::make_unique<CallExprAST>(idName, std::move(args));
    }

    std::unique_ptr<ExprAST> Parser::parsePrimary() {
        switch (lexer.getCurToken()) {
            default:
                return logError("unknown token when expecting an expression");
            case tok_identifier:
                return parseIdentifierExpr();
            case tok_number:
                return parseNumberExpr();
            case '(':
                return parseParenExpr();
        }
    }

    std::unique_ptr<ExprAST> Parser::parseBinOpRHs(int exprPrec, std::unique_ptr<ExprAST> LHS) {
        // If this is binop, find its precedence.
        while (true) {
            int tokPrec = getTokPrecedence();

            // If this is a binop that binds at least as tightly as the current binop,
            // consume it, otherwise we are done.
            if (tokPrec < exprPrec)
                return LHS;

            // Okay, we know this is a binop.
            int binOp = lexer.getCurToken();
            lexer.getNextToken(); // eat binop.

            // Parse the primary expression after the binary operator.
            auto RHS = parsePrimary();
            if (!RHS)
                return nullptr;

            // If BinOp binds less tightly with RHS than the operator after RHS, let
            // the pending operator take RHS as its LHS.
            int nextPrec = getTokPrecedence();
            if (tokPrec < nextPrec) {
                RHS = parseBinOpRHs(tokPrec + 1, std::move(RHS));
                if (!RHS)
                    return nullptr;
            }

            // Merge LHS/RHS.
            LHS = std::make_unique<BinaryExprAST>(binOp, std::move(LHS), std::move(RHS));
        }
    }

    std::unique_ptr<PrototypeAST> Parser::parsePrototype() {
        if (lexer.getCurToken() != tok_identifier)
            return logErrorP("Expected function name in prototype");

        std::string functionName = lexer.getId().str();
        lexer.getNextToken();

        if (lexer.getCurToken() != '(')
            return logErrorP("Expected '(' in prototype");

        std::vector<std::string> ArgNames;
        while (lexer.getNextToken() == tok_identifier)
            ArgNames.push_back(lexer.getId().str());
        if (lexer.getCurToken() != ')')
            return logErrorP("Expected ')' in prototype");

        // eat ')'
        lexer.getNextToken();

        return std::make_unique<PrototypeAST>(functionName, std::move(ArgNames));
    }

    std::unique_ptr<FunctionAST> Parser::parseDefinition() {
        // eat def.
        lexer.getNextToken();
        auto proto = parsePrototype();
        if (!proto)
            return nullptr;

        if (auto e = parseExpression())
            return std::make_unique<FunctionAST>(std::move(proto), std::move(e));
        return nullptr;
    }

    std::unique_ptr<FunctionAST> Parser::parseTopLevelExpr() {
        if (auto e = parseExpression()) {
            // Make an anonymous proto.
            auto proto = std::make_unique<PrototypeAST>("__anon_expr", std::vector<std::string>());
            return std::make_unique<FunctionAST>(std::move(proto), std::move(e));
        }
        return nullptr;
    }

    std::unique_ptr<PrototypeAST> Parser::parseExtern() {
        lexer.getNextToken();
        return parsePrototype();
    }

    void Parser::parse(llvm::ExitOnError err, std::unique_ptr<llvm::orc::KaleidoscopeJIT> jit) {
        lexer.getNextToken();
        while (true) {
            fprintf(stderr, "ready>");
            switch (lexer.getCurToken()) {
                case tok_eof:
                    return;
                case ';': // ignore top-level semicolons.
                    lexer.getNextToken();
                    break;
                case tok_def:
                    if (auto FnAst = parseDefinition()) {
                        if (auto *FnIR = FnAst->codegen()) {
                            fprintf(stderr, "Read function definition:");
                            FnIR->print(errs());
                            fprintf(stderr, "\n");
                        }
                    } else {
                        lexer.getNextToken();
                    }
                    break;
                case tok_extern:
                    if (auto ProtoAST = parseExtern()) {
                        if (auto *FnIR = ProtoAST->codegen()) {
                            fprintf(stderr, "Read extern:");
                            FnIR->print(errs());
                            fprintf(stderr, "\n");
                        }
                    } else {
                        lexer.getNextToken();
                    }
                    break;
                default:
                    if (auto FnAST = parseTopLevelExpr()) {
                        if (auto *FnIR = FnAST->codegen()) {
                            // Create a ResourceTracjer to track JIT'd memory allocated to our
                            // anonymous expression -- that way we can free it after executing.
                            auto RT = jit->getMainJITDylib().createResourceTracker();

//                            auto TSM = orc::ThreadSafeModule(std::move())
                        }
                    } else {
                        lexer.getNextToken();
                    }
                    break;
            }
        }
    }

}