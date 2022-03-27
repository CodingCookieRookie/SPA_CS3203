#pragma once

#include "RelationshipInstruction.h"

class ParentInstruction : public RelationshipInstruction {
protected:
	PqlReference lhsRef;
	PqlReference rhsRef;
	EvaluatedTable execute() override;
	EvaluatedTable helperHandleTwoIntegers();
	EvaluatedTable helperHandleOneInt(PqlReferenceType lhsRefType, PqlReferenceType rhsRefType);
	EvaluatedTable helperHandleTwoStmtsMaybeWildcard();
	EvaluatedTable helperHandleTwoWildcards();

public:
	ParentInstruction(PqlReference lhsRef, PqlReference rhsRef);
};
