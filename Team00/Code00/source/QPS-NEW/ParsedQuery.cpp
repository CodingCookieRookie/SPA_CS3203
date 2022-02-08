#include "ParsedQuery.h"

bool ParsedQuery::isDeclared(const std::string& synonym) {
	return declarations.find(synonym) != declarations.end();
}

ParsedQuery::ParsedQuery(const std::vector<PQL_VARIABLE>& allDeclarations,
	const std::vector<std::string>& allColumns,
	const std::vector<ParsedRelationship>& allRelationships,
	const std::vector<ParsedPattern>& allPatterns) {
	for (const PQL_VARIABLE& variable : allDeclarations) {
		PQL_VARIABLE_TYPE variableType = variable.first;
		std::string variableName = variable.second;
		if (isDeclared(variableName)) {
			throw SPAException(std::string("Repeated variable declaration found"));
		}
		declarations[variableName] = variableType;
	}
	for (const std::string& column : allColumns) {
		if (!isDeclared(column)) {
			throw SPAException(std::string("Undeclared variable found"));
		}
		columns.push_back(column);
	}
	for (const ParsedRelationship& relationship : allRelationships) {
		PqlReference lhs = relationship.getLhs();
		if (lhs.first == PqlReferenceType::synonym) {
			std::string lhsVarName = lhs.second;
			if (!isDeclared(lhsVarName)) {
				throw SPAException(std::string(""));
			}
		}
		PqlReference rhs = relationship.getRhs();
		if (rhs.first == PqlReferenceType::synonym) {
			std::string rhsVarName = rhs.second;
			if (!isDeclared(rhsVarName)) {
				throw SPAException(std::string(""));
			}
		}
		relationships.push_back(relationship);
	}
}

std::unordered_map<std::string, PQL_VARIABLE_TYPE> ParsedQuery::getDeclarations() {
	return declarations;
}

std::vector<std::string> ParsedQuery::getColumns() {
	return columns;
}

std::vector<ParsedRelationship> ParsedQuery::getRelationships() {
	return relationships;
}