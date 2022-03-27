#pragma once

#include "Instruction.h"

class RelationshipInstruction : public Instruction {
private:
	PqlRelationshipType pqlRelationshipType;
	PqlReference lhsRef;
	PqlReference rhsRef;

public:
	RelationshipInstruction();
	RelationshipInstruction(PqlRelationshipType pqlRSType, PqlReference lhs, PqlReference rhs);
	virtual EvaluatedTable execute() = 0;
};
