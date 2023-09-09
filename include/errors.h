#ifndef OD_ERROR_H
#define OD_ERROR_H

#include <memory>
#include <string>
#include <vector>

#include "sysexits.h"

class Error : public std::exception {
	public:
		Error() = default;
		explicit Error(const std::string& what) : what_(what){};
		explicit Error(std::string&& what) : what_(std::move(what)){};

		const char* what() const noexcept override;

		uint8_t exit_code = -1;  // if error should cause exit, this should be used.
	protected:
		mutable std::string what_;
};


struct Diagnostic {
	size_t starts_at;
	size_t ends_at;
	int line;
	std::string message;
};
  
#endif
