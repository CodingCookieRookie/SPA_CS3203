#include "QPSCommons.h"

ParsedRelationship::ParsedRelationship(PqlRelationshipType relationshipType,
	PqlReference lhsRef, PqlReference rhsRef)
	: relationshipType(relationshipType), lhsRef(lhsRef), rhsRef(rhsRef) {}

PqlRelationshipType ParsedRelationship::getRelationshipType() const {
	return relationshipType;
}

PqlReference ParsedRelationship::getLhs() const {
	return lhsRef;
}

PqlReference ParsedRelationship::getRhs() const {
	return rhsRef;
}

ParsedPattern::ParsedPattern(std::string& synonym, PqlReference entRef,
	PqlExpression expressionSpec, int numOfArgs)
	: ParsedPattern(synonym, PqlPatternType::Pattern, entRef, expressionSpec, numOfArgs) {}

ParsedPattern::ParsedPattern(std::string& synonym, PqlPatternType patternType,
	PqlReference entRef, PqlExpression expressionSpec, int numOfArgs)
	: synonym(synonym), patternType(patternType), entRef(entRef),
	expressionSpec(expressionSpec), numOfArgs(numOfArgs) {}

std::string ParsedPattern::getSynonym() const {
	return synonym;
}

PqlPatternType ParsedPattern::getPatternType() const {
	return patternType;
}

PqlReference ParsedPattern::getEntRef() const {
	return entRef;
}

PqlExpression ParsedPattern::getExpression() const {
	return expressionSpec;
}

int ParsedPattern::getNumOfArgs() const {
	return numOfArgs;
}

/* Default constructor used while parsing
Temporarily assign attribType as unvalidated because
we delay type validation to the validator */
ParsedWith::ParsedWith(PqlReference lhs, PqlReference rhs)
	: ParsedWith(lhs, rhs, PqlAttributeType::Unvalidated) {}

ParsedWith::ParsedWith(PqlReference lhs, PqlReference rhs, PqlAttributeType attribType)
	: lhs(lhs), rhs(rhs), attribType(attribType) {}

PqlReference ParsedWith::getLhs() const {
	return lhs;
}

PqlReference ParsedWith::getRhs() const {
	return rhs;
}

PqlAttributeType ParsedWith::getAttribType() const {
	return attribType;
}

bool isSynonymRef(PqlReference reference) {
	return reference.first == PqlReferenceType::Synonym;
}

bool isStatementEntity(PqlEntityType entityType) {
	/* TODO: refactor this into a table */
	return entityType == PqlEntityType::Assign ||
		entityType == PqlEntityType::Stmt ||
		entityType == PqlEntityType::If ||
		entityType == PqlEntityType::Print ||
		entityType == PqlEntityType::Read ||
		entityType == PqlEntityType::Stmt ||
		entityType == PqlEntityType::While ||
		entityType == PqlEntityType::Call;
}
