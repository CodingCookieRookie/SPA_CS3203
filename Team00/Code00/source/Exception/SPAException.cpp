#include "SPAException.h"

SPAException::SPAException(const std::string& reason) : reason(reason) {}

const char* SPAException::what() const noexcept {
	return reason.c_str();
}

/* LexerException */
const std::string LexerException::INVALID_INT = "Integer cannot start with 0";

LexerException::LexerException(const std::string& reason) : SPAException(reason) {}

/* ParserException */
const std::string ParserException::MISSING_SEMICOLON = "Semicolon is missing.";
const std::string ParserException::MISSING_VAR_NAME = "Variable name is missing.";
const std::string ParserException::MISSING_WHITESPACE = "Whitespace is missing.";

ParserException::ParserException(const std::string& reason) : SPAException(reason) {}
