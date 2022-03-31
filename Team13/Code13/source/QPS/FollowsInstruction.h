#pragma once

#include "RelationshipInstruction.h"

class FollowsInstruction : public RelationshipInstruction {
protected:
	EvaluatedTable execute() override;
	EvaluatedTable helperHandleTwoIntegers();
	EvaluatedTable helperHandleOneInt(PqlReferenceType lhsRefType, PqlReferenceType rhsRefType);
	EvaluatedTable helperHandleTwoStmtsMaybeWildcard();
	EvaluatedTable helperHandleTwoWildcards();

public:
	FollowsInstruction(PqlReference lhsRef, PqlReference rhsRef);
};
