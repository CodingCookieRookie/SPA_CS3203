#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../Exception/SPAException.h"
#include "Clause.h"
#include "ParsedRelationship.h"
#include "ParsedPattern.h"
#include "ParsedWith.h"
#include "ParsedGetAll.h"
#include "QPSCommons.h"

class ParsedQuery {
private:
	static const std::unordered_set<PqlReferenceType> STMT_REF;
	static const std::unordered_set<PqlReferenceType> ENT_REF;
	static const std::unordered_set<PqlReferenceType> STMT_REF_NO_WILDCARD;
	static const std::unordered_set<PqlReferenceType> ENT_REF_NO_WILDCARD;

	static const std::unordered_set<EntityType> STMT_ENTITIES;
	static const std::unordered_set<EntityType> ASSIGN_ENTITIES;
	static const std::unordered_set<EntityType> VAR_ENTITIES;
	static const std::unordered_set<EntityType> PROC_ENTITIES;

	/* These two unordered_maps validate the type of argument, e.g. stmtRef := '_' | synonym | INTEGER
	The leftArg map also provides a mapping from the generic Modifies/Uses to the specific P or S variants */
	static const std::unordered_map<PqlRelationshipType,
		std::vector<std::pair<PqlRelationshipType, std::unordered_set<PqlReferenceType>>>> VALID_LEFT_ARGS;
	static const std::unordered_map<PqlRelationshipType, std::unordered_set<PqlReferenceType>> VALID_RIGHT_ARGS;

	/* If the argument is a synonym, these two unordered_maps validate the design entity that the synonym is declared as */
	static const std::unordered_map<PqlRelationshipType, std::unordered_set<EntityType>> VALID_LEFT_DESIGN_ENTITIES;
	static const std::unordered_map<PqlRelationshipType, std::unordered_set<EntityType>> VALID_RIGHT_DESIGN_ENTITIES;

	/* Tables for type-validating the pattern clause */
	static const std::unordered_map<EntityType, PqlPatternType> VALID_PATTERN_SYN_DESIGN_ENTITIES;
	static const std::unordered_map<PqlPatternType, std::unordered_set<PqlReferenceType>> VALID_PATTERN_ENT_REF;
	static const std::unordered_map<PqlPatternType, std::unordered_set<EntityType>> VALID_PATTERN_ENT_REF_ENTITIES;
	static const std::unordered_map<PqlPatternType, std::unordered_set<PqlExpressionType>> VALID_PATTERN_EXPR_SPEC;
	static const std::unordered_map<PqlPatternType, int> VALID_PATTERN_NUM_OF_ARGS;

	/* Tables for type-validating the with-clause */
	static const std::unordered_set<PqlReferenceType> ATTRIB_REFERENCES;
	static const std::unordered_map<EntityType, std::unordered_set<PqlReferenceType>> VALID_ATTRIBS;
	static const std::unordered_map<PqlReferenceType, PqlAttributeType> REF_TO_ATTRIB_MAP;

	std::unordered_map<std::string, EntityType> declarations;
	std::unordered_set<std::string> columns;
	std::vector<PqlReference> attributes;
	std::vector<ParsedRelationship> relationships;
	std::vector<ParsedPattern> patterns;
	std::vector<ParsedWith> withs;

	bool isDeclared(const std::string& synonym);
	EntityType getType(std::string& synonym);

	void populateDeclarations(const std::vector<PqlEntity>& allDeclarations);
	void populateColumns(const std::vector<PqlReference>& allColumns);
	void populateRelationships(const std::vector<ParsedRelationship>& allRelationships);
	void populatePatterns(const std::vector<ParsedPattern>& allPatterns);
	void populateWiths(const std::vector<ParsedWith>& allWiths);
public:

	/* Constructor for a ParsedQuery object */
	ParsedQuery(const std::vector<PqlEntity>& allDeclarations,
		const std::vector<PqlReference>& allColumns,
		const std::vector<ParsedRelationship>& allRelationships,
		const std::vector<ParsedPattern>& allPatterns,
		const std::vector<ParsedWith>& allWiths);

	bool isStmtSubtype(PqlReference ref);
	static ProjectionType getProjectionType(std::vector<PqlReference> attributesProjected);
	static bool isClausePresent(ParsedQuery& parsedQuery);

	std::unordered_map<std::string, EntityType> getDeclarations();
	std::unordered_set<std::string> getColumns();
	std::vector<PqlReference> getAttributes();
	std::vector<ParsedRelationship> getRelationships();
	std::vector<ParsedPattern> getPatterns();
	std::vector<ParsedWith> getWiths();
};
