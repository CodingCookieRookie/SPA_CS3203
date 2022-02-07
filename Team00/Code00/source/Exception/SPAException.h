#pragma once
#include <exception>
#include <string>

// Base class that we inherit for all SPA-related exceptions
class SPAException : std::exception {
private:
	std::string reason;
public:
	SPAException(std::string& reason);
	const char* what() const noexcept override;
};