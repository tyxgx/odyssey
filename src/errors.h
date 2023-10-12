#ifndef __OD_ERROR_H__
#define __OD_ERROR_H__

#include <memory>
#include <string>
#include <vector>

#include "sysexits.h"

enum ErrorType {
	ERROR_PARSER,
	ERROR_LEXER,
};


struct Diagnostic {
	size_t starts_at;
	size_t ends_at;
	int line;
	std::string message;
	enum ErrorType err_t;
	/* std::string line_content; */
	void print_diagnostic();
};

class LexerError : public std::exception {
	public:
		LexerError() = default;
		explicit LexerError(const std::string& what) : what_(what){};
		explicit LexerError(std::string&& what) : what_(std::move(what)){};

		const char* what() const noexcept override;

		uint8_t exit_code = -1;  // if error should cause exit, this should be used.
	protected:
		mutable std::string what_;
};

class ParserError : public std::exception {
	public:
		ParserError() = default;
		explicit ParserError(const std::string& what) : what_(what){};
		explicit ParserError(std::string&& what) : what_(std::move(what)){};

		const char* what() const noexcept override;

		uint8_t exit_code = -1;  // if error should cause exit, this should be used.
	protected:
		mutable std::string what_;
};

  
#endif
