#pragma once

#include "RelationshipInstruction.h"

class ParentStarInstruction : public RelationshipInstruction {
protected:
	PqlReference lhsRef;
	PqlReference rhsRef;
	EvaluatedTable execute() override;
	EvaluatedTable helperHandleTwoIntegers();
	EvaluatedTable helperHandleOneInt(PqlReferenceType lhsRefType, PqlReferenceType rhsRefType);
	EvaluatedTable helperHandleTwoStmtsMaybeWildcard();
	EvaluatedTable helperHandleTwoWildcards();

public:
	ParentStarInstruction(PqlReference lhsRef, PqlReference rhsRef);
};
