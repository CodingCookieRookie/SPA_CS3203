#pragma once

#include "Clause.h"

class ParsedRelationship : public Clause {
private:
	PqlRelationshipType relationshipType;
	PqlReference lhsRef;
	PqlReference rhsRef;
public:
	ParsedRelationship(PqlRelationshipType relationshipType,
		PqlReference lhsRef, PqlReference rhsRef);
	PqlRelationshipType getRelationshipType() const;
	PqlReference getLhs() const;
	PqlReference getRhs() const;
	Instruction* toInstruction() const;
};
