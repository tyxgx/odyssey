#include <fstream>
#include <iostream>

#include "code_visitor.h"
#include "lexer.h"
#include "parser.h"

int main(int argc, const char **argv) {
    if (argc == 2) {
        std::string filename = argv[1];
        std::ifstream ifs(filename);
        std::string content((std::istreambuf_iterator<char>(ifs)),
                            (std::istreambuf_iterator<char>()));
        Lexer lexer = Lexer(content);
        std::vector<struct Token> tokens = lexer.lex();
        for (auto diagnostic : lexer._diagnostics) {
            diagnostic.print_diagnostic();
        }
        if (!lexer._diagnostics.empty()) exit(1);

#ifdef LEXER_DISASS
        for (struct Token token : tokens) {
            std::cout << "TOKEN_TYPE: " << token.tt
                      << " CONTENT: " << token.content << std::endl;
        }
#endif

        Parser parser = Parser(tokens);
        auto expr = parser.parse();
        CodeVisitor visitor;
        if (!expr) {
            std::cout << "Parser returned NULL" << std::endl;
        }
        for (auto diagnostic : parser._diagnostics) {
            diagnostic.print_diagnostic();
        }
        if (!parser._diagnostics.empty()) exit(1);

        auto code_gen = expr->codegen(visitor);
        code_gen->print(llvm::errs());

        if (llvm::verifyModule(*visitor.TheModule))
            llvm::errs() << *visitor.TheModule;

        std::cout << std::endl;
        visitor.TheModule->print(llvm::errs(), nullptr);

    } else {
        std::cout << "Usage: ./ody <source_file>" << std::endl;
    }

    return 0;
}
