#pragma once

#include "Instruction.h"
#include "../PKB/Attribute.h"

class WithInstruction : public Instruction {
protected:
	PqlReference lhs;
	PqlReference rhs;
	EntityType lhsEntity;
	EntityType rhsEntity;
	PqlAttributeType attribType;

	EvaluatedTable handleInt();
	EvaluatedTable handleString();
public:
	WithInstruction(PqlReference lhs, PqlReference rhs, EntityType lhsEntity, EntityType rhsEntity, PqlAttributeType attribType);

	/* Main entry method for executing instruction */
	EvaluatedTable execute() override;
};

class WithStringInstruction : public WithInstruction {
public:
	WithStringInstruction(PqlReference lhs, PqlReference rhs, EntityType lhsEntity, EntityType rhsEntity);
	EvaluatedTable execute() override;
};

class WithIntegerInstruction : public WithInstruction {
public:
	WithIntegerInstruction(PqlReference lhs, PqlReference rhs, EntityType lhsEntity, EntityType rhsEntity);
	EvaluatedTable execute() override;
};
