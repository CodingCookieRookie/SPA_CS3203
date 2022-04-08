#pragma once

#include "RelationshipInstruction.h"

class FollowsInstruction : public RelationshipInstruction {
protected:

	EvaluatedTable helperHandleTwoIntegers();
	EvaluatedTable helperHandleOneInt(PqlReferenceType lhsRefType, PqlReferenceType rhsRefType);
	EvaluatedTable helperHandleTwoStmtsMaybeWildcard();
	EvaluatedTable helperHandleTwoWildcards();

public:
	EvaluatedTable execute() override;
	FollowsInstruction(PqlReference lhsRef, PqlReference rhsRef, PKBGetter* pkbGetter);
};
