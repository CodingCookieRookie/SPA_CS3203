#pragma once

#include <string>

#include "../Common/Types.h"

enum class InstructionType {
	getAll, relationship, pattern
};

enum class RelationshipInstructionType {
	getFollows, getFollowsT, getParent, getParentT,
	getUsesS, getUsesP, getModifiesS, getModifiesP,
	getUses, getModifies
};

enum class PatternInstructionType {
	getPattern
};

enum class PqlRelationshipType {
	Follows, FollowsT, Parent, ParentT,
	UsesS, UsesP, ModifiesS, ModifiesP,
	Calls, CallsT, Next, NextT,
	Affects, AffectsT,
	Uses, Modifies
};

enum class PqlPatternType {
	Pattern, PatternA, PatternI, PatternW
};

enum class PqlReferenceType {
	synonym, wildcard, integer, ident,
	procName, varName, value, stmtNum
};

enum class PqlExpressionType {
	full, partial, wildcard
};

enum class PqlAttributeType {
	string, integer
};

typedef std::pair<PqlEntityType, std::string> PQL_VARIABLE;

typedef std::pair<PqlReferenceType, std::string> PqlReference;

typedef std::pair<PqlExpressionType, std::string> PqlExpression;

class ParsedRelationship {
private:
	PqlRelationshipType relationshipType;
	PqlReference lhsRef;
	PqlReference rhsRef;
public:
	ParsedRelationship(PqlRelationshipType relationshipType,
		PqlReference lhsRef, PqlReference rhsRef);
	PqlRelationshipType getRelationshipType() const;
	PqlReference getLhs() const;
	PqlReference getRhs() const;
};

class ParsedPattern {
private:
	std::string synonym;
	PqlPatternType patternType;
	PqlReference entRef;
	PqlExpression expressionSpec;
	int numOfArgs;
public:
	ParsedPattern(std::string& synonym, PqlReference entRef,
		PqlExpression expressionSpec, int numOfArgs);
	ParsedPattern(std::string& synonym, PqlPatternType patternType,
		PqlReference entRef, PqlExpression expressionSpec, int numOfArgs);
	std::string getSynonym() const;
	PqlPatternType getPatternType() const;
	PqlReference getEntRef() const;
	PqlExpression getExpression() const;
	int getNumOfArgs() const;
};

class ParsedWith {
private:
	PqlReference lhs;
	PqlReference rhs;
public:
	ParsedWith(PqlReference lhs, PqlReference rhs);
	PqlReference getLhs() const;
	PqlReference getRhs() const;
};

bool isSynonymRef(PqlReference reference);
bool isStatementEntity(PqlEntityType entityType);
