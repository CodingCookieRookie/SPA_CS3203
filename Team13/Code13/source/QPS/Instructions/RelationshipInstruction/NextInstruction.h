#pragma once

#include "RelationshipInstruction.h"

class NextInstruction : public RelationshipInstruction {
protected:
	EvaluatedTable helperHandleTwoIntegers();
	EvaluatedTable helperHandleOneInt(PqlReferenceType lhsRefType, PqlReferenceType rhsRefType);
	EvaluatedTable helperHandleTwoStmtsMaybeWildcard();
	EvaluatedTable helperHandleTwoWildcards();

public:
	NextInstruction(PqlReference lhsRef, PqlReference rhsRef, PKBGetter* pkbGetter);
	EvaluatedTable execute() override;
};
