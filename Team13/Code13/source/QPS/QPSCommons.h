#pragma once

#include <string>

enum class PqlEntityType {
	Stmt, Read, Print, Call, While,
	If, Assign, Variable, Constant, Procedure
};

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
	synonym, wildcard, integer, ident
};

enum class PqlExpressionType {
	full, partial, wildcard
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
	PqlRelationshipType getRelationshipType();
	PqlReference getLhs() const;
	PqlReference getRhs() const;
};

class ParsedPattern {
private:
	std::string synonym;
	PqlPatternType patternType;
	PqlReference entRef;
	PqlExpression expressionSpec;
public:
	ParsedPattern(std::string& synonym,
		PqlReference entRef, PqlExpression expressionSpec);
	ParsedPattern(std::string& synonym, PqlPatternType patternType,
		PqlReference entRef, PqlExpression expressionSpec);
	std::string getSynonym() const;
	PqlPatternType getPatternType() const;
	PqlReference getEntRef() const;
	PqlExpression getExpression() const;
};

bool isEntRef(PqlReference reference);
bool isStmtRef(PqlReference reference);
bool isWildcardRef(PqlReference reference);
bool isSynonymRef(PqlReference reference);

bool isUsesRelationship(ParsedRelationship relationship);
bool isModifiesRelationship(ParsedRelationship relationship);
bool isFollowsRelationship(ParsedRelationship relationship);
bool isParentRelationship(ParsedRelationship relationship);

bool isStatementEntity(PqlEntityType entityType);
bool isProcedureEntity(PqlEntityType entityType);
