#include "SPAException.h"

SPAException::SPAException(std::string& reason) : reason(reason) {}

const char* SPAException::what() const noexcept {
	return reason.c_str();
}
