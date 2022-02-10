#pragma once
#include <string>

enum class PqlEntityType {
    Stmt, Read, Print, Call, While,
    If, Assign, Variable, Constant, Procedure
};

enum class InstructionType {
    getAllStmt, getAllRead, getAllPrint, getAllCall, getAllWhile,
    getAllIf, getAllAsgn, getAllVar, getAllConst, getAllProc
};

// The Uses and Modifies are generic Uses/Modifies,
// since we cannot disambiguate while parsing.
// Hence, we re-assign the S and P relationships
// when populating the ParsedQuery Object
// To note for iter 2 and beyond
enum class PqlRelationshipType {
    Follows, FollowsT, Parent, ParentT,
    UsesS, UsesP, ModifiesS, ModifiesP,
    Uses, Modifies
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
    ParsedRelationship();
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
    ParsedPattern();
    std::string getSynonym() const;
    PqlReference getEntRef() const;
    PqlExpression getExpression() const;
};

bool isEntRef(PqlReference reference);
bool isStmtRef(PqlReference reference);
bool isWildcardRef(PqlReference reference);
bool isSynonymRef(PqlReference reference);

bool isUsesRelationship(ParsedRelationship relationship);
bool isModifiesRelationship(ParsedRelationship relationship);

bool isStatementEntity(PqlEntityType entityType);
bool isProcedureEntity(PqlEntityType entityType);
