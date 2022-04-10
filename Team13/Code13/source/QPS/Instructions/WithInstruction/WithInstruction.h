#pragma once

#include "../Instruction.h"
#include "../../../PKB/Attribute/Attribute.h"

class WithInstruction : public Instruction {
protected:
	PqlReference lhs;
	PqlReference rhs;
	EntityType lhsEntity;
	EntityType rhsEntity;
	PqlAttributeType attribType;
	PKBGetter* pkbGetter;

	EvaluatedTable handleInt();
	EvaluatedTable handleString();
public:
	WithInstruction(PqlReference lhs, PqlReference rhs, EntityType lhsEntity, EntityType rhsEntity, PqlAttributeType attribType, PKBGetter* pkbGetter);

	/* Main entry method for executing instruction */
	EvaluatedTable execute() override;

	std::unordered_set<std::string> getSynonyms();
};

class WithStringInstruction : public WithInstruction {
public:
	WithStringInstruction(PqlReference lhs, PqlReference rhs, EntityType lhsEntity, EntityType rhsEntity, PKBGetter* pkbGetter);
	EvaluatedTable execute() override;
};

class WithIntegerInstruction : public WithInstruction {
public:
	WithIntegerInstruction(PqlReference lhs, PqlReference rhs, EntityType lhsEntity, EntityType rhsEntity, PKBGetter* pkbGetter);
	EvaluatedTable execute() override;
};
