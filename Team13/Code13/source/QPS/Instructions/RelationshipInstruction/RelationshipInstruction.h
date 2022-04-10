#pragma once

#include "../Instruction.h"

class RelationshipInstruction : public Instruction {
protected:
	PqlReference lhsRef;
	PqlReference rhsRef;
	PKBGetter* pkbGetter;
	NextTProcessor* nextTProcessor = 0;
	AffectsProcessor* affectsProcessor = 0;
	AffectsTProcessor* affectsTProcessor = 0;

public:
	RelationshipInstruction(PqlReference lhs, PqlReference rhs, PKBGetter* pkbGetter);
	RelationshipInstruction(PqlReference lhs, PqlReference rhs,
		AffectsProcessor* affectsProcessor, PKBGetter* pkbGetter);
	RelationshipInstruction(PqlReference lhs, PqlReference rhs,
		AffectsTProcessor* affectsTProcessor, PKBGetter* pkbGetter);
	RelationshipInstruction(PqlReference lhs, PqlReference rhs,
		NextTProcessor* nextTProcessor, PKBGetter* pkbGetter);
	virtual EvaluatedTable execute() = 0;
	std::unordered_set<std::string> getSynonyms() override;
};
