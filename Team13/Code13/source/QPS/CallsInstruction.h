#pragma once

#include "RelationshipInstruction.h"

class CallsInstruction : public RelationshipInstruction {
protected:
	EvaluatedTable helperHandleTwoIdents();
	EvaluatedTable helperHandleOneIdent(PqlReferenceType lhsRefType, PqlReferenceType rhsRefType);
	EvaluatedTable helperHandleTwoProcMaybeWildcard();
	EvaluatedTable helperHandleTwoWildcards();

public:
	CallsInstruction(PqlReference lhsRef, PqlReference rhsRef, PKBGetter* pkbGetter);
	EvaluatedTable execute() override;
};
