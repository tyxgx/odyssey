/// Lexer includes

#ifndef OD_LEXER_H
#define OD_LEXER_H


enum TokenKind {

	// Special Tokens
	TOKEN_EOF,
	TOKEN_ERROR

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

	// Data types
	TOKEN_INT,
	TOKEN_DOUBLE,
	TOKEN_STRING,
	TOKEN_TRUE,
	TOKEN_FALSE
	TOKEN_VOID,

	// Keywords
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
	int len;
	size_t starts_at;
	size_t ends_at;
	int line;
};

class Lexer {
	private:
		// helpers
		char advance();
		char peek();
		char peek_next();
		void add_token();
		void skip_whitespaces();

		// variables
		size_t _current_pos;
		size_t _start_pos;
		// position on the current line, not the source
		size_t _pos_line_current;
		size_t _pos_line_start;
		int _line;
		std::string _source;
		std::unordered_map<std::string, enum TokenKind> _token_map;

	public:
		std::vector<struct Token> lex();
		Lexer(std::string source);

}


#endif
