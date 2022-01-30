#pragma once
#include <vector>
#include <unordered_map>

#include "QPSCommons.h"

class ParsedQuery {
private:
	std::unordered_map<std::string, PQL_VARIABLE_TYPE> declarations;
	std::vector<std::string> columns;
public:
	ParsedQuery(std::vector<PQL_VARIABLE> allDeclarations, std::vector<std::string> columns);
	std::unordered_map<std::string, PQL_VARIABLE_TYPE> getDeclarations();
	std::vector<std::string> getColumns();
};
