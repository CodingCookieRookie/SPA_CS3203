#pragma once

#include "RelationshipInstruction.h"

class CallsStarInstruction : public RelationshipInstruction {
protected:
	EvaluatedTable execute() override;
	EvaluatedTable helperHandleTwoIdents();
	EvaluatedTable helperHandleOneIdent(PqlReferenceType lhsRefType, PqlReferenceType rhsRefType);
	EvaluatedTable helperHandleTwoProcMaybeWildcard();
	EvaluatedTable helperHandleTwoWildcards();

public:
	CallsStarInstruction(PqlReference lhsRef, PqlReference rhsRef);
};
