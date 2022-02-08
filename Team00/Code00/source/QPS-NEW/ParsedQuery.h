#pragma once
#include <vector>
#include <unordered_map>

#include "QPSCommons.h"
#include "../Exception/SPAException.h"

class ParsedQuery {
private:
	std::unordered_map<std::string, PqlEntityType> declarations;
	std::vector<std::string> columns;
	std::vector<ParsedRelationship> relationships;
	std::vector<ParsedPattern> patterns;

	bool isDeclared(const std::string& synonym);
	PqlEntityType getType(std::string& synonym);

	void populateDeclarations(
		const std::vector<PQL_VARIABLE>& allDeclarations);
	void populateColumns(
		const std::vector<std::string>& allColumns);
	void populateRelationships(
		const std::vector<ParsedRelationship>& allRelationships);
	void populatePatterns(
		const std::vector<ParsedPattern>& allPatterns);
public:

	/* Constructor for a ParsedQuery object */
	ParsedQuery(const std::vector<PQL_VARIABLE>& allDeclarations,
		const std::vector<std::string>& allColumns,
		const std::vector<ParsedRelationship>& allRelationships,
		const std::vector<ParsedPattern>& allPatterns);


	std::unordered_map<std::string, PqlEntityType> getDeclarations();
	std::vector<std::string> getColumns();
	std::vector<ParsedRelationship> getRelationships();
	std::vector<ParsedPattern> getPatterns();
};
