#pragma once

#include "RelationShipInstruction.h"

class ParentInstruction : public RelationshipInstruction {
protected:
	PqlReference lhsRef;
	PqlReference rhsRef;
	ParentInstruction(PqlReference lhsRef, PqlReference rhsRef);
	EvaluatedTable execute() override;
};
