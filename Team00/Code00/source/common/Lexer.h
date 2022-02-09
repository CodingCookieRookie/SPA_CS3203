#pragma once
#include <string>

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
	bool match(const std::string& pattern);
	bool reachedEnd();
};
