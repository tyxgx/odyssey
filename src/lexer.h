/// Lexer includes

#ifndef OD_LEXER_H
#define OD_LEXER_H

#include <unordered_map>
#include <vector>
#include <string>

#include "errors.h"

enum TokenKind {

	// Special Tokens
	TOKEN_EOF,
	TOKEN_ERROR,

	// Binary Operators
	TOKEN_PLUS,
	TOKEN_MINUS,
	TOKEN_SLASH,
	TOKEN_STAR,
	TOKEN_EQUAL,
	TOKEN_EQUAL_EQUAL,
	TOKEN_BANG_EQUAL,
	TOKEN_GREATER,
	TOKEN_LESS,
	TOKEN_LESS_EQUAL,
	TOKEN_GREATER_EQUAL,
	
	// Unary operator
	TOKEN_BANG,

	// Punctuation
	TOKEN_COLON,
	TOKEN_SEMICOLON,
	TOKEN_COMMA,
	TOKEN_LEFT_PAREN,
	TOKEN_RIGHT_PAREN,
	TOKEN_LEFT_CURLY,
	TOKEN_RIGHT_CURLY,

	// identifier
	TOKEN_ID,

	// Data types
	TOKEN_INT,
	TOKEN_DOUBLE,
	TOKEN_STRING,
	TOKEN_TRUE,
	TOKEN_BOOL,
	TOKEN_FALSE,
	TOKEN_VOID,
	TOKEN_NIL,

	// Keywords
	TOKEN_PRINT,
	TOKEN_FUNC,
	TOKEN_FOR,
	TOKEN_WHILE,
	TOKEN_LET,
	TOKEN_IF,
	TOKEN_ELSE,
	TOKEN_ELIF
};

struct Token {
	enum TokenKind tt;
	size_t len;
	size_t starts_at;
	size_t ends_at;
	int line;
	std::string content;

	std::string dump();
};

class Lexer {
	private:
		// helpers
		char _advance();
		char _peek();
		char _peek_next();
		void _add_token(enum TokenKind tt);
		void _skip_whitespaces();
		void _number();
		void _string();
		void _identifier();
		void _identifier_type();
		bool _match(char expected);
		bool _at_end();
		void _error(std::string msg);
		void _scan_token();

		// variables
		size_t _current_pos;
		size_t _start_pos;
		// position on the current line, not the source
		size_t _pos_line_current;
		size_t _pos_line_start;
		int _line;
		bool _error_in_lexer;

		std::vector<struct Token> _token_list;
		std::string _source;
		std::unordered_map<std::string, enum TokenKind> _token_map;

	public:
		std::vector<struct Token> lex();
		std::vector<struct Diagnostic> _diagnostics;
		Lexer(std::string source);
};


#endif
