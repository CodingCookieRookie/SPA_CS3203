#pragma once

#include <string>
#include <unordered_map>

const static std::unordered_map<std::string, int> designEntities = {
	{ "stmt", 0 },
	{ "read", 1 },
	{ "print", 2 },
	{ "call", 3 },
	{ "while", 4 },
	{ "if", 5 },
	{ "assign", 6 },
	{ "variable", 7 },
	{ "constant", 8 },
	{ "procedure", 9 },
};

static bool checkIfWordIsDesignEntity(std::string word) {
	return designEntities.find(word) != designEntities.end();
}
