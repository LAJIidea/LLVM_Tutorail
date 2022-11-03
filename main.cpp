#include "Parse.h"

using namespace toy;
int main() {
    Lexer lexer;
    Parser parser(lexer);
    // Install standard binary operators.
    //1 is the lowest precedence.
    parser.addBinOpPrecedence('<', 10);
    parser.addBinOpPrecedence('+', 20);
    parser.addBinOpPrecedence('-', 20);
    parser.addBinOpPrecedence('*', 40); //highest.

    // Primary the first token.
    fprintf(stderr, "ready>");
    parser.parse();

    return 0;
}
