#include "Parse.h"
#include "KaleidoscopeJIT.h"

using namespace toy;
using namespace llvm;
using namespace llvm::orc;
int main() {
    Lexer lexer;
    Parser parser(lexer);
    // Install standard binary operators.
    //1 is the lowest precedence.
    parser.addBinOpPrecedence('<', 10);
    parser.addBinOpPrecedence('+', 20);
    parser.addBinOpPrecedence('-', 20);
    parser.addBinOpPrecedence('*', 40); //highest.

    ExitOnError ExitOnErr;
    auto jit = ExitOnErr(KaleidoscopeJIT::Create());

    // Make the module, which holds all the code.
    InitializeModule(std::move(jit));

    // Primary the first token.
    fprintf(stderr, "ready>");
    parser.parse(ExitOnErr, std::move(jit));

    showErrors();

    return 0;
}
