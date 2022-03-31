#pragma once

#include "RelationshipInstruction.h"

class FollowsStarInstruction : public RelationshipInstruction {
protected:
	EvaluatedTable execute() override;
	EvaluatedTable helperHandleTwoIntegers();
	EvaluatedTable helperHandleOneInt(PqlReferenceType lhsRefType, PqlReferenceType rhsRefType);
	EvaluatedTable helperHandleTwoStmtsMaybeWildcard();
	EvaluatedTable helperHandleTwoWildcards();

public:
	FollowsStarInstruction(PqlReference lhsRef, PqlReference rhsRef);
};
