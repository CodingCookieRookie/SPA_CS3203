#pragma once

#include "Instruction.h"
#include "../PKB/Attribute.h"

class WithInstruction : public Instruction {
private:
	PqlReference lhs;
	PqlReference rhs;
	PqlEntityType lhsEntity;
	PqlEntityType rhsEntity;
	PqlAttributeType attribType;

	EvaluatedTable handleInt();
	EvaluatedTable handleString();
public:
	WithInstruction(PqlReference lhs, PqlReference rhs, PqlEntityType lhsEntity, PqlEntityType rhsEntity, PqlAttributeType attribType);

	/* Main entry method for executing instruction */
	EvaluatedTable execute() override;
};
