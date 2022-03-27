#pragma once

#include "Clause.h"

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
