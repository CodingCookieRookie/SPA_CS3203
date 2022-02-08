#include "QPSCommons.h"

ParsedRelationship::ParsedRelationship(PqlRelationshipType relationshipType,
	PqlReference lhsRef, PqlReference rhsRef)
	: relationshipType(relationshipType), lhsRef(lhsRef), rhsRef(rhsRef)
{}

PqlReference ParsedRelationship::getLhs() const {
	return lhsRef;
}

PqlReference ParsedRelationship::getRhs() const {
	return rhsRef;
}

ParsedPattern::ParsedPattern(std::string& synonym, PqlReference entRef, PqlExpression expressionSpec)
	: synonym(synonym), entRef(entRef), expressionSpec(expressionSpec)
{}
