#include "ParsedQuery.h"

ParsedQuery::ParsedQuery(const std::vector<PQL_VARIABLE>& allDeclarations,
	const std::vector<std::string>& allColumns) {
	for (const PQL_VARIABLE& variable : allDeclarations) {
		PQL_VARIABLE_TYPE variableType = variable.first;
		std::string variableName = variable.second;
		if (declarations.find(variableName) != declarations.end()) {
			throw SPAException(std::string("Repeated variable declaration found"));
		}
		declarations[variableName] = variableType;
	}
	for (const std::string& column : allColumns) {
		if (declarations.find(column) == declarations.end()) {
			throw SPAException(std::string("Undeclared variable found"));
		}
		columns.push_back(column);
	}
}

std::unordered_map<std::string, PQL_VARIABLE_TYPE> ParsedQuery::getDeclarations() {
	return declarations;
}

std::vector<std::string> ParsedQuery::getColumns() {
	return columns;
}
