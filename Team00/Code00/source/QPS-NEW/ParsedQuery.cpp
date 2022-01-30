#include "ParsedQuery.h"

ParsedQuery::ParsedQuery(std::vector<PQL_VARIABLE> allDeclarations, std::vector<std::string> columns)
	: columns(columns) {
	for (const PQL_VARIABLE& variable : allDeclarations) {
		PQL_VARIABLE_TYPE variableType = variable.first;
		std::string variableName = variable.second;
		if (declarations.find(variableName) != declarations.end()) {
			// Previously declared variable -> throw exception
			return;
		}
		declarations[variableName] = variableType;
	}
}

std::unordered_map<std::string, PQL_VARIABLE_TYPE> ParsedQuery::getDeclarations() {
	return declarations;
}

std::vector<std::string> ParsedQuery::getColumns() {
	return columns;
}
