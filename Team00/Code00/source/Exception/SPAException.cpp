#include "SPAException.h"

SPAException::SPAException(const std::string& reason) : reason(reason) {}

const char* SPAException::what() const noexcept {
	return reason.c_str();
}

/* LexerException */
const std::string LexerException::INVALID_INT = "Integer cannot start with 0";

LexerException::LexerException(const std::string& reason) : SPAException(reason) { }
