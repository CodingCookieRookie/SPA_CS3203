#include "../QPS-NEW/PQLLexer.h"

PQLLexer::PQLLexer(const std::string& source)
	: source(source), index(0), length(source.length()) { }

void PQLLexer::advance() {
	while (index < length && isspace(source[index])) {
		index++;
	}
	return;
}

std::string PQLLexer::nextInteger() {
	advance();
	if (index >= length) {
		return std::string();
	}
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

std::string PQLLexer::nextName() {
	advance();
	if (index >= length) {
		return std::string();
	}
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

std::string PQLLexer::nextWhitespace() {
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

bool PQLLexer::match(const std::string& pattern) {
	advance();
	if (index + pattern.length() > length) {
		return false;
	}
	for (size_t i = 0; i < pattern.length(); i++) {
		if (pattern[i] != source[index + i]) {
			return false;
		}
	}
	index += pattern.length();
	return true;
}

bool PQLLexer::reachedEnd() {
	return (index == length);
}
