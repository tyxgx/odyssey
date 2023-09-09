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


};




#endif
