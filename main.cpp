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

    // Make the module, which holds all the code.
    InitializeModule();

    // Primary the first token.
    fprintf(stderr, "ready>");
    parser.parse();

    showErrors();

    return 0;
}
