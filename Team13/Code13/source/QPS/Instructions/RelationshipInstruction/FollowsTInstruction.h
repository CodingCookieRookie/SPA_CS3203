#pragma once

#include "RelationshipInstruction.h"

class FollowsTInstruction : public RelationshipInstruction {
protected:
	EvaluatedTable helperHandleTwoIntegers();
	EvaluatedTable helperHandleOneInt(PqlReferenceType lhsRefType, PqlReferenceType rhsRefType);
	EvaluatedTable helperHandleTwoStmtsMaybeWildcard();
	EvaluatedTable helperHandleTwoWildcards();

public:
	FollowsTInstruction(PqlReference lhsRef, PqlReference rhsRef, PKBGetter* pkbGetter);
	EvaluatedTable execute() override;
};
