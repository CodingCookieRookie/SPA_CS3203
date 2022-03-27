#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../Exception/SPAException.h"
#include "Clause.h"
#include "ParsedRelationship.h"
#include "ParsedPattern.h"
#include "QPSCommons.h"

class ParsedQuery {
private:
	static const std::unordered_set<PqlReferenceType> stmtRef;
	static const std::unordered_set<PqlReferenceType> entRef;
	static const std::unordered_set<PqlReferenceType> stmtRefNoWildcard;
	static const std::unordered_set<PqlReferenceType> entRefNoWildcard;

	static const std::unordered_set<PqlEntityType> stmtEntities;
	static const std::unordered_set<PqlEntityType> assignEntities;
	static const std::unordered_set<PqlEntityType> varEntities;
	static const std::unordered_set<PqlEntityType> procEntities;

	/* These two unordered_maps validate the type of argument, e.g. stmtRef := '_' | synonym | INTEGER
	The leftArg map also provides a mapping from the generic Modifies/Uses to the specific P or S variants */
	static const std::unordered_map<PqlRelationshipType,
		std::vector<std::pair<PqlRelationshipType, std::unordered_set<PqlReferenceType>>>> validLeftArgs;
	static const std::unordered_map<PqlRelationshipType, std::unordered_set<PqlReferenceType>> validRightArgs;

	/* If the argument is a synonym, these two unordered_maps validate the design entity that the synonym is declared as */
	static const std::unordered_map<PqlRelationshipType, std::unordered_set<PqlEntityType>> validLeftDesignEntities;
	static const std::unordered_map<PqlRelationshipType, std::unordered_set<PqlEntityType>> validRightDesignEntities;

	/* Tables for type-validating the pattern clause */
	static const std::unordered_map<PqlEntityType, PqlPatternType> validPatternSynDesignEntities;
	static const std::unordered_map<PqlPatternType, std::unordered_set<PqlReferenceType>> validPatternEntRef;
	static const std::unordered_map<PqlPatternType, std::unordered_set<PqlEntityType>> validPatternEntRefEntities;
	static const std::unordered_map<PqlPatternType, std::unordered_set<PqlExpressionType>> validPatternExprSpec;
	static const std::unordered_map<PqlPatternType, int> validPatternNumOfArgs;

	/* Tables for type-validating the with-clause */
	static const std::unordered_set<PqlReferenceType> attribReferences;
	static const std::unordered_map<PqlEntityType, std::unordered_set<PqlReferenceType>> validAttribs;
	static const std::unordered_map<PqlReferenceType, PqlAttributeType> refToAttribMap;

	std::unordered_map<std::string, PqlEntityType> declarations;
	std::unordered_set<std::string> columns;
	std::vector<PqlReference> attributes;
	std::vector<ParsedRelationship> relationships;
	std::vector<ParsedPattern> patterns;
	std::vector<ParsedWith> withs;

	bool isDeclared(const std::string& synonym);
	PqlEntityType getType(std::string& synonym);

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

	std::unordered_map<std::string, PqlEntityType> getDeclarations();
	std::unordered_set<std::string> getColumns();
	std::vector<PqlReference> getAttributes();
	std::vector<ParsedRelationship> getRelationships();
	std::vector<ParsedPattern> getPatterns();
	std::vector<ParsedWith> getWiths();
};
