#pragma once
#include <vector>
#include <unordered_map>

#include "QPSCommons.h"
#include "../Exception/SPAException.h"

class ParsedQuery {
private:
	std::unordered_map<std::string, PQL_VARIABLE_TYPE> declarations;
	std::vector<std::string> columns;
	std::vector<ParsedRelationship> relationships;
	std::vector<ParsedPattern> patterns;
	bool isDeclared(const std::string& synonym);
public:

	/* Constructor for a ParsedQuery object */
	ParsedQuery(const std::vector<PQL_VARIABLE>& allDeclarations,
		const std::vector<std::string>& allColumns,
		const std::vector<ParsedRelationship>& allRelationships,
		const std::vector<ParsedPattern>& allPatterns);


	std::unordered_map<std::string, PQL_VARIABLE_TYPE> getDeclarations();
	std::vector<std::string> getColumns();
};
