#include "QPSCommons.h"

ParsedRelationship::ParsedRelationship(PqlRelationshipType relationshipType,
    PqlReference lhsRef, PqlReference rhsRef)
    : relationshipType(relationshipType), lhsRef(lhsRef), rhsRef(rhsRef)
{}

ParsedRelationship::ParsedRelationship() {}

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

ParsedPattern::ParsedPattern() {}

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

bool isStatementEntity(PqlEntityType entityType) {
    return entityType == PqlEntityType::Assign
        || entityType == PqlEntityType::Stmt
        || entityType == PqlEntityType::If
        || entityType == PqlEntityType::Print
        || entityType == PqlEntityType::Read
        || entityType == PqlEntityType::Stmt
        || entityType == PqlEntityType::While;
}

bool isProcedureEntity(PqlEntityType entityType) {
    return entityType == PqlEntityType::Procedure;
}
