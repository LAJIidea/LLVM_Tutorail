//
// Created by BY210033 on 2022/11/1.
//

#ifndef LLVM_TUTORAIL_PARSER_H
#define LLVM_TUTORAIL_PARSER_H

#include "AST.h"
#include "Lexer.h"

namespace toy {

    class Parser {
    public:
        /// Create a Parser for the supplied lexer.
        explicit Parser(Lexer &lexer): lexer(lexer) {}

        /// Parse a full Module. A module is a list of function definitions.
//        std::unique_ptr<

    private:
        Lexer &lexer;
    };

}

#endif //LLVM_TUTORAIL_PARSER_H
