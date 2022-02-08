#pragma once
#include <string>

enum class PQL_VARIABLE_TYPE {
	STMT, READ, PRINT, CALL, WHILE,
	IF, ASSIGN, VARIABLE, CONSTANT, PROCEDURE
};

enum class INSTRUCTION_TYPE {
	getAllStmt, getAllVar, getAllAsgn
};

enum class PqlRelationshipType {
	Follows, FollowsT, Parent, ParentT,
	Uses, Modifies
};

enum class PqlReferenceType {
	synonym, wildcard, integer, ident
};

enum class PqlExpressionType {
	full, partial, wildcard
};

typedef std::pair<PQL_VARIABLE_TYPE, std::string> PQL_VARIABLE;

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
	PqlReference entRef;
	PqlExpression expressionSpec;
public:
	ParsedPattern(std::string& synonym,
		PqlReference entRef, PqlExpression expressionSpec);
};
