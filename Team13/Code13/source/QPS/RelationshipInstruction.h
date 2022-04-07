#pragma once

#include "Instruction.h"
#include "AffectsProcessor.h"
#include "AffectsTProcessor.h"

class RelationshipInstruction : public Instruction {
protected:
	PqlReference lhsRef;
	PqlReference rhsRef;
	NextTProcessor* nextTProcessor;
	AffectsProcessor* affectsProcessor;
	AffectsTProcessor* affectsTProcessor;

public:
	RelationshipInstruction();
	RelationshipInstruction(PqlReference lhs, PqlReference rhs);
	RelationshipInstruction(PqlReference lhs, PqlReference rhs, NextTProcessor* nextTProcessor);
	RelationshipInstruction(PqlReference lhs, PqlReference rhs, AffectsProcessor* affectsProcessor);
	RelationshipInstruction(PqlReference lhs, PqlReference rhs, AffectsTProcessor* affectsTProcessor);
	virtual EvaluatedTable execute() = 0;
	std::unordered_set<std::string> getSynonyms() override;
};
