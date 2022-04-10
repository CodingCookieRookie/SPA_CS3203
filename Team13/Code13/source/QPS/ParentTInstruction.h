#pragma once

#include "RelationshipInstruction.h"

class ParentTInstruction : public RelationshipInstruction {
protected:
	EvaluatedTable helperHandleTwoIntegers();
	EvaluatedTable helperHandleOneInt(PqlReferenceType lhsRefType, PqlReferenceType rhsRefType);
	EvaluatedTable helperHandleTwoStmtsMaybeWildcard();
	EvaluatedTable helperHandleTwoWildcards();

public:
	ParentTInstruction(PqlReference lhsRef, PqlReference rhsRef, PKBGetter* pkbGetter);
	EvaluatedTable execute() override;
};
