//
// Created by BY210033 on 2022/11/1.
//
#include "Parse.h"

namespace toy {

    int Parser::getTkPrecedence() {
        return 0;
    }

    std::unique_ptr<ExprAST> Parser::logError(const char *str) {
        return std::unique_ptr<ExprAST>();
    }

    std::unique_ptr<PrototypeAST> Parser::logErrorP(const char *str) {
        return std::unique_ptr<PrototypeAST>();
    }

    std::unique_ptr<ExprAST> Parser::parseExpression() {
        return std::unique_ptr<ExprAST>();
    }

    std::unique_ptr<ExprAST> Parser::parseNumberExpr() {
        return std::unique_ptr<ExprAST>();
    }

    std::unique_ptr<ExprAST> Parser::parseParenExpr() {
        return std::unique_ptr<ExprAST>();
    }

    std::unique_ptr<ExprAST> Parser::parseIdentifierExpr() {
        return std::unique_ptr<ExprAST>();
    }

    std::unique_ptr<ExprAST> Parser::parseBinOpRHs(int exprPrec, std::unique_ptr<ExprAST> LHS) {
        return std::unique_ptr<ExprAST>();
    }

    std::unique_ptr<PrototypeAST> Parser::parsePrototype() {
        return std::unique_ptr<PrototypeAST>();
    }

    std::unique_ptr<FunctionAST> Parser::parseDefinition() {
        return std::unique_ptr<FunctionAST>();
    }

    std::unique_ptr<FunctionAST> Parser::parseTopLevelExpr() {
        return std::unique_ptr<FunctionAST>();
    }

    std::unique_ptr<PrototypeAST> Parser::parseExtern() {
        return std::unique_ptr<PrototypeAST>();
    }

}