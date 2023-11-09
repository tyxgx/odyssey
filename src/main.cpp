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
        auto program = parser.parse();
        CodeVisitor visitor;

        for (auto diagnostic : parser._diagnostics) {
            diagnostic.print_diagnostic();
        }
        if (!parser._diagnostics.empty()) exit(1);

        std::string code;

        llvm::raw_string_ostream code_stream(code);
        /* std::error_code err_code; */
        /* llvm::raw_fd_ostream out_ll("./out.ll", err_code); */
        for(auto &stmt: program) {
            auto codegen = stmt->Expression->codegen(visitor);
            code_stream << *codegen;
            code_stream.flush();
        }

        if (visitor.had_error_somewhere) {
            for (auto diagnostic : visitor.diagnostics_) {
                diagnostic.print_diagnostic();
            }
            exit(1);
        }

        auto fn_type = llvm::FunctionType::get(visitor.Builder->getInt32Ty(), false);
        auto fn = visitor.TheModule->getFunction("main");

        if (fn == nullptr) {
            // create the function
            fn =
                llvm::Function::Create(fn_type, llvm::Function::ExternalLinkage,
                        "main", *(visitor.TheModule));
            verifyFunction(*fn);
        }

        auto entry =
            llvm::BasicBlock::Create(*(visitor.TheContext), "entry", fn);
        visitor.Builder->SetInsertPoint(entry);
        visitor.Builder->CreateRet(visitor.Builder->getInt32(0));

        code_stream << *visitor.TheModule;
        code_stream.flush();
        std::cout << code << std::endl;
        /* visitor.TheModule->print(out_ll, nullptr); */
        /* out_ll.close(); */
    } else {
        std::cout << "Usage: ./ody <source_file>" << std::endl;
    }

    return 0;
}
