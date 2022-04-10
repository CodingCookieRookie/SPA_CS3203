#pragma once

#include "RelationshipInstruction.h"

class CallsTInstruction : public RelationshipInstruction {
protected:
	EvaluatedTable helperHandleTwoIdents();
	EvaluatedTable helperHandleOneIdent(PqlReferenceType lhsRefType, PqlReferenceType rhsRefType);
	EvaluatedTable helperHandleTwoProcMaybeWildcard();
	EvaluatedTable helperHandleTwoWildcards();

public:
	CallsTInstruction(PqlReference lhsRef, PqlReference rhsRef, PKBGetter* pkbGetter);
	EvaluatedTable execute() override;
};
