#pragma once

#include "Instruction.h"
#include "QPSCommons.h"
#include "WithInstruction.h"
#include "ModifiesInstruction.h"
#include "UsesInstruction.h"

class Clause {
public:
	virtual Instruction* toInstruction() {
		return nullptr;
	}
};

class ParsedWith : public Clause {
private:
	PqlReference lhs;
	PqlReference rhs;
	PqlEntityType lhsEntity;
	PqlEntityType rhsEntity;
	PqlAttributeType attribType;
public:
	ParsedWith(PqlReference lhs, PqlReference rhs);
	ParsedWith(PqlReference lhs, PqlReference rhs, PqlEntityType lhsEntity, PqlEntityType rhsEntity, PqlAttributeType attribType);
	PqlReference getLhs() const;
	PqlReference getRhs() const;
	PqlAttributeType getAttribType() const;
	Instruction* toInstruction() const;
};
