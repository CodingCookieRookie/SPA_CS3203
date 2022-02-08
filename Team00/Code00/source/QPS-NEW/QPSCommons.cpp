#include "QPSCommons.h"

ParsedRelationship::ParsedRelationship(PqlRelationshipType relationshipType,
	PqlReference lhsRef, PqlReference rhsRef)
	: relationshipType(relationshipType), lhsRef(lhsRef), rhsRef(rhsRef)
{}

PqlRelationshipType ParsedRelationship::getRelationshipType() {
	return relationshipType;
}

PqlReference ParsedRelationship::getLhs() const {
	return lhsRef;
}

PqlReference ParsedRelationship::getRhs() const {
	return rhsRef;
}

ParsedPattern::ParsedPattern(std::string& synonym, PqlReference entRef, PqlExpression expressionSpec)
	: synonym(synonym), entRef(entRef), expressionSpec(expressionSpec)
{}

std::string ParsedPattern::getSynonym() const {
	return synonym;
}

PqlReference ParsedPattern::getEntRef() const{
	return entRef;
}

PqlExpression ParsedPattern::getExpression() const {
	return expressionSpec;
}
