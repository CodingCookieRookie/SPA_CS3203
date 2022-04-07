#pragma once

#include "RelationshipInstruction.h"

class NextStarInstruction : public RelationshipInstruction {
private:
	CFGProcessor* nextTProcessor;

protected:
	EvaluatedTable helperHandleTwoIntegers();
	EvaluatedTable helperHandleOneInt(PqlReferenceType lhsRefType, PqlReferenceType rhsRefType);
	EvaluatedTable helperHandleTwoStmtsMaybeWildcard();
	EvaluatedTable helperHandleTwoWildcards();

public:
	NextStarInstruction(PqlReference lhsRef, PqlReference rhsRef, CFGProcessor* nextTProcessor);
	EvaluatedTable execute() override;
};
