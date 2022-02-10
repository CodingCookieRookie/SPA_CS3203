#pragma once
#include <exception>
#include <string>

// Base class that we inherit for all SPA-related exceptions
class SPAException : std::exception {
private:
	const std::string reason;
public:
	SPAException(const std::string& reason);
	const char* what() const noexcept override;
};

class LexerException : public SPAException {
public:
	static const std::string INVALID_INT;

	LexerException(const std::string& reason);
};
