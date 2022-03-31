#pragma once

#include "Instruction.h"

class RelationshipInstruction : public Instruction {
protected:
	PqlReference lhsRef;
	PqlReference rhsRef;

public:
	RelationshipInstruction(PqlReference lhs, PqlReference rhs);
	virtual EvaluatedTable execute() = 0;
	std::unordered_set<std::string> getSynonyms() override;
};
