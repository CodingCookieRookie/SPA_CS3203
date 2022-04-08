#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../Exception/SPAException.h"
#include "Instruction.h"
#include "ParsedRelationship.h"
#include "ParsedPattern.h"
#include "ParsedWith.h"
#include "ParsedGetAll.h"
#include "QPSCommons.h"

class ParsedQuery {
private:
	bool tupleSelect;
	std::vector<PqlEntity> declarations;
	std::vector<PqlReference> columns;
	std::vector<ParsedRelationship> relationships;
	std::vector<ParsedPattern> patterns;
	std::vector<ParsedWith> withs;

	bool isDeclared(const std::string& synonym);
	EntityType getType(std::string& synonym);

	std::unordered_map<std::string, EntityType> generateDeclarationMap();


	std::unordered_set<std::string> getColumns(const std::unordered_map<std::string, EntityType>& declarationMap);
	std::vector<PqlReference> ParsedQuery::getAttributes(const std::unordered_map<std::string, EntityType>& declarationMap);

	std::vector<Instruction*> getSuchThatInstructions(
		const std::unordered_map<std::string, EntityType>& declarationMap, const Processors& processors, PKBGetter* pkbGetter);
	std::vector<Instruction*> getPatternInstructions(
		const std::unordered_map<std::string, EntityType>& declarationMap, PKBGetter* pkbGetter);
	std::vector<Instruction*> getWithInstructions(
		const std::unordered_map<std::string, EntityType>& declarationMap, PKBGetter* pkbGetter);
public:

	/* Constructor for a ParsedQuery object */
	ParsedQuery(const std::vector<PqlEntity>& allDeclarations,
		const std::vector<PqlReference>& allColumns,
		const std::vector<ParsedRelationship>& allRelationships,
		const std::vector<ParsedPattern>& allPatterns,
		const std::vector<ParsedWith>& allWiths,
		bool tupleSelect);

	bool isStmtSubtype(PqlReference ref);
	static ProjectionType getProjectionType(std::vector<PqlReference> attributesProjected);
	static bool isClausePresent(ParsedQuery& parsedQuery);

	std::unordered_map<std::string, EntityType> getDeclarations();
	std::unordered_set<std::string> getColumns();
	std::vector<ParsedRelationship> getRelationships();
	std::vector<ParsedPattern> getPatterns();
	std::vector<ParsedWith> getWiths();

	std::vector<Instruction*> getClauseInstructions(const Processors& processors, PKBGetter* pkbGetter);
	std::vector<Instruction*> getSelectInstructions(PKBGetter* pkbGetter);
	std::vector<PqlReference> getAttributes();
};
