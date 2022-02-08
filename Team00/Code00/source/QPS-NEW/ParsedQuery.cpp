#include "ParsedQuery.h"

bool ParsedQuery::isDeclared(const std::string& synonym) {
	return declarations.find(synonym) != declarations.end();
}

PqlEntityType ParsedQuery::getType(std::string& synonym) {
	return declarations.at(synonym);
}

void ParsedQuery::populateDeclarations(
	const std::vector<PQL_VARIABLE>& allDeclarations) {
	for (const PQL_VARIABLE& variable : allDeclarations) {
		PqlEntityType variableType = variable.first;
		std::string variableName = variable.second;
		if (isDeclared(variableName)) {
			throw SPAException(std::string("Repeated variable declaration found"));
		}
		declarations[variableName] = variableType;
	}
}

void ParsedQuery::populateColumns(const std::vector<std::string>& allColumns) {
	for (const std::string& column : allColumns) {
		if (!isDeclared(column)) {
			throw SPAException(std::string("Undeclared variable found"));
		}
		columns.push_back(column);
	}
}

void ParsedQuery::populateRelationships(const std::vector<ParsedRelationship>& allRelationships) {
	for (const ParsedRelationship& relationship : allRelationships) {
		PqlReference lhs = relationship.getLhs();
		PqlReference rhs = relationship.getRhs();

		if (isModifiesRelationship(relationship)) {
			if (!isStmtRef(lhs)) {
				// throw exception
			}
			if (isSynonymRef(lhs)) {
				std::string lhsVarName = lhs.second;
				if (!isDeclared(lhsVarName)) {
					throw SPAException(std::string(""));
				}
			}
			if (!isEntRef(rhs)) {
				// throw exception
			}
			if (isSynonymRef(rhs)) {
				std::string rhsVarName = rhs.second;
				if (!isDeclared(rhsVarName)) {
					throw SPAException(std::string(""));
				}
			}
			relationships.push_back(
				ParsedRelationship(PqlRelationshipType::ModifiesS, lhs, rhs));
		} else if (isUsesRelationship(relationship)) {
			if (!isStmtRef(lhs)) {
				// throw exception
			}
			if (isSynonymRef(lhs)) {
				std::string lhsVarName = lhs.second;
				if (!isDeclared(lhsVarName)) {
					throw SPAException(std::string(""));
				}
			}
			if (!isEntRef(rhs)) {
				// throw exception
			}
			if (isSynonymRef(rhs)) {
				std::string rhsVarName = rhs.second;
				if (!isDeclared(rhsVarName)) {
					throw SPAException(std::string(""));
				}
			}
			relationships.push_back(
				ParsedRelationship(PqlRelationshipType::UsesS, lhs, rhs));
		} else {
			if (!isStmtRef(lhs)) {
				// throw exception
			}
			if (isSynonymRef(lhs)) {
				std::string lhsVarName = lhs.second;
				if (!isDeclared(lhsVarName)) {
					throw SPAException(std::string(""));
				}
			}
			if (!isStmtRef(rhs)) {
				// throw exception
			}
			if (isSynonymRef(rhs)) {
				std::string rhsVarName = rhs.second;
				if (!isDeclared(rhsVarName)) {
					throw SPAException(std::string(""));
				}
			}
			relationships.push_back(relationship);
		}
	}
}

void ParsedQuery::populatePatterns(const std::vector<ParsedPattern>& allPatterns) {
	for (const ParsedPattern& pattern : allPatterns) {
		std::string synAssign = pattern.getSynonym();
		if (!isDeclared(synAssign)) {

		}
		if (getType(synAssign) != PqlEntityType::Assign) {

		}
		PqlReference ref = pattern.getEntRef();
		PqlReferenceType refType = ref.first;
		if (!isEntRef(ref)) {
		}
		patterns.push_back(pattern);
	}
}

ParsedQuery::ParsedQuery(const std::vector<PQL_VARIABLE>& allDeclarations,
	const std::vector<std::string>& allColumns,
	const std::vector<ParsedRelationship>& allRelationships,
	const std::vector<ParsedPattern>& allPatterns) {
	populateDeclarations(allDeclarations);
	populateColumns(allColumns);
	populateRelationships(allRelationships);
	populatePatterns(allPatterns);
}

std::unordered_map<std::string, PqlEntityType> ParsedQuery::getDeclarations() {
	return declarations;
}

std::vector<std::string> ParsedQuery::getColumns() {
	return columns;
}

std::vector<ParsedRelationship> ParsedQuery::getRelationships() {
	return relationships;
}

std::vector<ParsedPattern> ParsedQuery::getPatterns() {
	return patterns;
}