#pragma once

#include "RelationshipInstruction.h"

class CallsStarInstruction : public RelationshipInstruction {
protected:
	EvaluatedTable helperHandleTwoIdents();
	EvaluatedTable helperHandleOneIdent(PqlReferenceType lhsRefType, PqlReferenceType rhsRefType);
	EvaluatedTable helperHandleTwoProcMaybeWildcard();
	EvaluatedTable helperHandleTwoWildcards();

public:
	CallsStarInstruction(PqlReference lhsRef, PqlReference rhsRef, PKBGetter* pkbGetter);
	EvaluatedTable execute() override;
};
