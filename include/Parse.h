//
// Created by BY210033 on 2022/11/1.
//

#ifndef LLVM_TUTORAIL_PARSER_H
#define LLVM_TUTORAIL_PARSER_H

#include "AST.h"
#include "Lexer.h"

#include <map>

namespace toy {

    class Parser {
    public:
        /// Create a Parser for the supplied lexer.
        explicit Parser(Lexer &lexer): lexer(lexer) {}

        void addBinOpPrecedence(char op, int level);

        /// top ::= definition | external | expression | ';'
        void parse();

    private:
        Lexer &lexer;

        /// BinopPrecedence - This holds the precedence for each binary operator that is
        /// defined
        std::map<char, int> BinopPrecedence;

        /// Get the precedence of the pending binary operator token.
        int getTokPrecedence();

        /// These are little helper functions for error handling.
        std::unique_ptr<ExprAST> logError(const char  *str);

        std::unique_ptr<PrototypeAST> logErrorP(const char *str);

        /// expression ::= primary binoprhs
        std::unique_ptr<ExprAST> parseExpression();

        /// number-expr ::= number
        std::unique_ptr<ExprAST> parseNumberExpr();

        /// paren-expr ::= '(' expression ')'
        std::unique_ptr<ExprAST> parseParenExpr();

        /// identifier-expr ::= identifier
        ////                 |  identifier '(' expression* ')'
        std::unique_ptr<ExprAST> parseIdentifierExpr();

        /// primary ::= identifier-expr
        ///          |  number-expr
        ///          |  paren-expr
        std::unique_ptr<ExprAST> parsePrimary();

        /// binoprhs ::= ('+' primary)*
        std::unique_ptr<ExprAST> parseBinOpRHs(int exprPrec, std::unique_ptr<ExprAST> LHS);

        /// prototype ::= id '(' id* ')'
        std::unique_ptr<PrototypeAST> parsePrototype();

        /// definition ::= 'def' prototype expression
        std::unique_ptr<FunctionAST> parseDefinition();

        /// toplevel-expr ::= expression
        std::unique_ptr<FunctionAST> parseTopLevelExpr();

        /// external ::= 'extern' prototype
        std::unique_ptr<PrototypeAST> parseExtern();
    };

}

#endif //LLVM_TUTORAIL_PARSER_H
