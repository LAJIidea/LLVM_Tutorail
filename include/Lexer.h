//
// Created by BY210033 on 2022/11/1.
//

#ifndef LLVM_TUTORAIL_LEXER_H
#define LLVM_TUTORAIL_LEXER_H

#include <llvm/ADT/StringRef.h>

#include <memory>
#include <string>

namespace toy {

    /// Structure definition a location in a file.
    struct Location {
        std::shared_ptr<std::string> file;  ///< filename.
        int line;                           ///< line number.
        int col;                            ///< column number.
    };

    /// List of Token returned by the lexer.
    enum Token: int  {
        tok_eof = -1,

        // commands
        tok_def = -2,
        tok_extern = -3,

        // primary
        tok_identifier = -4,
        tok_number = -5
    };

    /**
     * The Lexer is a class providing all the facilities that the Parser expects.
     * It goes through the stream one token at a time and keeps track of the
     * location in the file for debugging purpose.
     * It relies on a subclass to provide a `readNextLine()` method. The subclass
     * can proceed by reading the next line from the standard input or from a
     * memory mapped file.
     * Also provide a `readNextChar()` method.
     */
    class Lexer {
    public:
        Lexer() = default;
        /// Create a lexer for the given filename. The filename is kept only for
        /// debugging purpose (attaching a location to a Token).
        Lexer(std::string filename);

        virtual ~Lexer() = default;

        /// Look at the current token in the stream.
        Token getCurToken() { return curTok; }

        /// Move to the next token in the stream and return it.
        Token getNextToken() { return curTok = getTok(); }

        /// Move to the next token in the stream, asserting on the current token
        /// matching the expectation.
        void consume(Token tok) {
            assert(tok == curTok && "consume Token mismatch expectation");
            getNextToken();
        }

        /// Return the current identifier (prereq: getCurToken() = tok_identifier)
        llvm::StringRef getId() {
            assert(curTok == tok_identifier);
            return identifierStr;
        }

        /// Return the current number (prereq: getCurToken() == tok_number)
        double getValue() {
            assert(curTok == tok_number);
            return numVal;
        }

        /// Return the location for the beginning of the current token.
        Location getLastLocation() { return lastLocation; }

        /// Return the current line in the file.
        int getLine() { return curLineNum; }

        /// Return the current column int he file
        int getCol() { return curCol; }

    private:
        /// Delegate to a derived class fetching the next line. Returns an empty
        /// string to signal end of file (EOF). Lines are expected to always finish
        /// with "\n"
        llvm::StringRef readNextLine();

        /// Return the next character from the stream. This manages the buffer for the
        /// current line and request the next line buffer to the derived class as
        /// needed.
        int getNextChar();

        /// Return the next token from standard input.
        Token getTok();

        /// Return the next token from file input.
        Token getFileTok();

        /// The last token read from the input.
        Token curTok = tok_eof;

        /// Location for `curTok`
        Location lastLocation;

        /// If the current Token is an identifier, this string contains the value.
        std::string identifierStr;

        /// If the current Token is a number, this contains the value.
        double numVal = 0;

        /// The last value returned by getNextChar(). We need to keep it around as we
        /// always need to read ahead one character to decide when to end a token and
        /// we can't put it back in the stream after reading from it.
        Token lastChar = Token(' ');

        /// Keep track of the current line number in the input stream
        int curLineNum = 0;

        /// keep track of the current column number in the input stream
        int curCol = 0;

        /// Buffer supplied by the derived class on calls to `readNextLine()`
        llvm::StringRef curLineBuffer = "\n";

    };

}

#endif //LLVM_TUTORAIL_LEXER_H
