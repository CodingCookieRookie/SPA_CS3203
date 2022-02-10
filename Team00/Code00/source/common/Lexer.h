#pragma once
#include <string>

#include "../Exception/SPAException.h"

class Lexer {
private:
	std::string source;
	size_t index;
	size_t length;
	void advance();
public:
	Lexer();
	Lexer(const std::string& source);
	std::string nextInteger();
	std::string nextName();
	std::string nextWhitespace();
	bool peek(const std::string& pattern);
	bool match(const std::string& pattern);
	bool reachedEnd();
};
