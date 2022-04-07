#include "RelationshipInstruction.h"

RelationshipInstruction::RelationshipInstruction() :
	nextTProcessor(NULL), affectsProcessor(NULL), affectsTProcessor(NULL) {}

RelationshipInstruction::RelationshipInstruction(PqlReference lhs, PqlReference rhs) :
	lhsRef(lhs), rhsRef(rhs), nextTProcessor(NULL), affectsProcessor(NULL), affectsTProcessor(NULL) {}

RelationshipInstruction::RelationshipInstruction(PqlReference lhs, PqlReference rhs, NextTProcessor* nextTProcessor) :
	lhsRef(lhs), rhsRef(rhs), nextTProcessor(nextTProcessor), affectsProcessor(NULL), affectsTProcessor(NULL) {}

RelationshipInstruction::RelationshipInstruction(PqlReference lhs, PqlReference rhs, AffectsProcessor* affectsProcessor) :
	lhsRef(lhs), rhsRef(rhs), affectsProcessor(affectsProcessor), affectsTProcessor(NULL), nextTProcessor(NULL) {}

RelationshipInstruction::RelationshipInstruction(PqlReference lhs, PqlReference rhs, AffectsTProcessor* affectsTProcessor) :
	lhsRef(lhs), rhsRef(rhs), affectsTProcessor(affectsTProcessor), affectsProcessor(NULL), nextTProcessor(NULL) {}

EvaluatedTable RelationshipInstruction::execute() {
	return EvaluatedTable();
}

std::unordered_set<std::string> RelationshipInstruction::getSynonyms() {
	std::unordered_set<std::string> results;
	if (isSynonymRef(lhsRef)) {
		results.insert(lhsRef.second);
	}
	if (isSynonymRef(rhsRef)) {
		results.insert(rhsRef.second);
	}
	return results;
}
