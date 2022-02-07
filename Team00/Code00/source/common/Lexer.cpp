#include "../common/Lexer.h"

Lexer::Lexer(const std::string& source) : source(source), index(0), length(source.length()) {}

void Lexer::advance() {
	if (index >= length) {
		return;
	}
	while (index < length && isspace(source[index])) {
		index++;
	}

	return;
}

std::string Lexer::nextInteger() {
	if (index >= length) {
		return std::string();
	}
	advance();
	if (!isdigit(source[index])) {
		return std::string();
	}
	std::string match;
	while (index < length && isdigit(source[index])) {
		match.push_back(source[index]);
		index++;
	}
	return match;
}

std::string Lexer::nextName() {
	if (index >= length) {
		return std::string();
	}
	advance();
	if (!isalpha(source[index])) {
		return std::string();
	}
	std::string match;
	while (index < length && isalnum(source[index])) {
		match.push_back(source[index]);
		index++;
	}
	return match;
}

std::string Lexer::nextWhitespace() {
	if (index >= length) {
		return std::string();
	}
	if (!isspace(source[index])) {
		return std::string();
	}
	std::string match;
	while (index < length && isspace(source[index])) {
		match.push_back(source[index]);
		index++;
	}
	return match;
}

bool Lexer::match(const std::string& pattern) {
	advance();

	if (!peek(pattern)) {
		return false;
	}

	index += pattern.length();
	return true;
}

bool Lexer::peek(const std::string& pattern) {
	advance();

	if (index + pattern.length() > length) {
		return false;
	}
	for (size_t i = 0; i < pattern.length(); i++) {
		if (pattern[i] != source[index + i]) {
			return false;
		}
	}
	return true;
}

bool Lexer::reachedEnd() {
	advance();
	return (index == length);
}
