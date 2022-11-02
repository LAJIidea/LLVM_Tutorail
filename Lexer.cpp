//
// Created by BY210033 on 2022/11/1.
//
#include "Lexer.h"

namespace toy {

    Lexer::Lexer(std::string filename)
        : lastLocation({std::make_shared<std::string>(std::move(filename)), 0, 0}){}

    llvm::StringRef Lexer::readNextLine() {
        return llvm::StringRef();
    }

    int Lexer::getNextChar() {
        return 0;
    }

    Token Lexer::getTok() {
        static int lastChar = ' ';

        // skip any whitespace.
        while (isspace(lastChar))
            lastChar = getchar();

        if (isalpha(lastChar)) { // identifier: [a-zA-Z][a-zA-Z0-9]
            identifierStr = lastChar;
            while (isalnum(lastChar = getchar()))
                identifierStr += lastChar;

            if (identifierStr == "def")
                return tok_def;
            if (identifierStr == "extern")
                return tok_extern;
            return tok_identifier;
        }

        if (isdigit(lastChar) || lastChar == '.') { // Number: [0-9]+
            std::string numStr;
            do {
                numStr += lastChar;
                lastChar = getchar();
            } while (isdigit(lastChar) || lastChar == '.');

            numVal = strtod(numStr.c_str(), nullptr);
            return tok_number;
        }

        if (lastChar == '#') {
            // Comment until end of line.
            do
                lastChar = getchar();
            while (lastChar != EOF && lastChar != '\n' && lastChar != '\r');

            if (lastChar != EOF)
                return getTok();
        }
        // check for end. Don't eat the EOF.
        if (lastChar == EOF)
            return tok_eof;

        // Otherwise, just return the character as its ascii value.
        Token thisChar = Token(lastChar);
        lastChar = Token(getNextToken());
        return thisChar;
    }

    Token Lexer::getFileTok() {
        return tok_eof;
    }

}