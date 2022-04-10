#include "RelationshipInstruction.h"

RelationshipInstruction::RelationshipInstruction(PqlReference lhs, PqlReference rhs, PKBGetter* pkbGetter) :
	lhsRef(lhs), rhsRef(rhs), pkbGetter(pkbGetter) {}

RelationshipInstruction::RelationshipInstruction(PqlReference lhs, PqlReference rhs,
	NextTProcessor* nextTProcessor, PKBGetter* pkbGetter) :
	lhsRef(lhs), rhsRef(rhs), pkbGetter(pkbGetter), nextTProcessor(nextTProcessor) {}

RelationshipInstruction::RelationshipInstruction(PqlReference lhs, PqlReference rhs,
	AffectsTProcessor* affectsTProcessor, PKBGetter* pkbGetter) :
	lhsRef(lhs), rhsRef(rhs), pkbGetter(pkbGetter), affectsTProcessor(affectsTProcessor) {}

RelationshipInstruction::RelationshipInstruction(PqlReference lhs, PqlReference rhs,
	AffectsProcessor* affectsProcessor, PKBGetter* pkbGetter) :
	lhsRef(lhs), rhsRef(rhs), pkbGetter(pkbGetter), affectsProcessor(affectsProcessor) {}

std::unordered_set<std::string> RelationshipInstruction::getSynonyms() {
	std::unordered_set<std::string> results;
	if (QPSCommons::isSynonymRef(lhsRef)) {
		results.insert(lhsRef.second);
	}
	if (QPSCommons::isSynonymRef(rhsRef)) {
		results.insert(rhsRef.second);
	}
	return results;
}
