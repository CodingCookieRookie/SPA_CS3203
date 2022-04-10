#pragma once

#include "RelationshipInstruction.h"

class NextStarInstruction : public RelationshipInstruction {
protected:
	EvaluatedTable helperHandleTwoIntegers();
	EvaluatedTable helperHandleOneInt(PqlReferenceType lhsRefType, PqlReferenceType rhsRefType);
	EvaluatedTable helperHandleTwoStmtsMaybeWildcard();
	EvaluatedTable helperHandleTwoWildcards();

public:
	NextStarInstruction(PqlReference lhsRef, PqlReference rhsRef, NextTProcessor* nextTProcessor, PKBGetter* pkbGetter);
	EvaluatedTable execute() override;
	QueryScore getDifficultyScore() override;
};
