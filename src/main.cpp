#include <fstream>
#include <iostream>
#include "../include/lexer.h"

int main(int argc, const char **argv) {

	if (argc == 2) {
		std::string filename = argv[1];
		std::ifstream ifs(filename);
		std::string content((std::istreambuf_iterator<char>(ifs)),
				(std::istreambuf_iterator<char>()));
		Lexer lexer = Lexer(content);

		try {
			std::vector<struct Token> tokens = lexer.lex();
			for (struct Token token : tokens) {
				std::cout << "TOKEN_TYPE: " << token.tt << " CONTENT: " << token.content << std::endl;
			}
		}
		catch(Error& e) {
			for(auto diagnostic : lexer._diagnostics) {
				std::cout << diagnostic.message << std::endl;
			}
		}
	}
	else {
		std::cout << "Usage: ./ody <source_file>" << std::endl;
	}

	return 0;
}
