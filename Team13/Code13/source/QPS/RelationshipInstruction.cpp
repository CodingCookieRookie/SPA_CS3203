#include "RelationshipInstruction.h"

RelationshipInstruction::RelationshipInstruction() {}

RelationshipInstruction::RelationshipInstruction(PqlReference lhs, PqlReference rhs) :
	lhsRef(lhs), rhsRef(rhs) {}

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
