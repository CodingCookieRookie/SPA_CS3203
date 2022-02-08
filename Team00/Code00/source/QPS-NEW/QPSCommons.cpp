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

bool isEntRef(PqlReference reference) {
	return
		reference.first == PqlReferenceType::synonym
		|| reference.first == PqlReferenceType::wildcard
		|| reference.first == PqlReferenceType::ident;
}

bool isStmtRef(PqlReference reference) {
	return
		reference.first == PqlReferenceType::synonym
		|| reference.first == PqlReferenceType::wildcard
		|| reference.first == PqlReferenceType::integer;
}

bool isWildcardRef(PqlReference reference) {
	return reference.first == PqlReferenceType::wildcard;
}

bool isSynonymRef(PqlReference reference) {
	return reference.first == PqlReferenceType::synonym;
}

bool isUsesRelationship(ParsedRelationship relationship) {
	return relationship.getRelationshipType() == PqlRelationshipType::Uses;
}

bool isModifiesRelationship(ParsedRelationship relationship) {
	return relationship.getRelationshipType() == PqlRelationshipType::Modifies;
}

bool isStatementEntity(PQL_VARIABLE_TYPE entityType) {
	return entityType == PQL_VARIABLE_TYPE::ASSIGN
		|| entityType == PQL_VARIABLE_TYPE::STMT
		|| entityType == PQL_VARIABLE_TYPE::IF
		|| entityType == PQL_VARIABLE_TYPE::PRINT
		|| entityType == PQL_VARIABLE_TYPE::READ
		|| entityType == PQL_VARIABLE_TYPE::STMT
		|| entityType == PQL_VARIABLE_TYPE::WHILE;
}

bool isProcedureEntity(PQL_VARIABLE_TYPE entityType) {
	return entityType == PQL_VARIABLE_TYPE::PROCEDURE;
}
