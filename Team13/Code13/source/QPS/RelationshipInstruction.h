#pragma once

#include "Instruction.h"

class RelationshipInstruction : public Instruction {
private:
	PqlRelationshipType pqlRelationshipType;
	PqlReference lhsRef;
	PqlReference rhsRef;

	EvaluatedTable handleModifiesS();
	EvaluatedTable handleModifiesP();
	EvaluatedTable handleUsesS();
	EvaluatedTable handleUsesP();
	EvaluatedTable handleFollows();
	EvaluatedTable handleFollowsT();
	EvaluatedTable handleParent(PqlRelationshipType pqlRsType);
	EvaluatedTable handleParentT();
	EvaluatedTable handleCalls(PqlRelationshipType pqlRsType);
	EvaluatedTable handleNext(PqlRelationshipType pqlRsType);

	/* Helper Handle methods */
	EvaluatedTable RelationshipInstruction::helperHandleTwoIntegers(PqlRelationshipType pqlRsType);
	EvaluatedTable helperHandleTwoIdents(PqlRelationshipType pqlRsType);
	EvaluatedTable helperHandleOneInt(
		PqlRelationshipType pqlRsType, PqlReferenceType lhsRefType, PqlReferenceType rhsRefType);
	EvaluatedTable RelationshipInstruction::helperHandleOneIdent(
		PqlRelationshipType pqlRsType, PqlReferenceType lhsRefType, PqlReferenceType rhsRefType);
	EvaluatedTable RelationshipInstruction::helperHandleTwoStmtsMaybeWildcard(PqlRelationshipType pqlRsType);
	EvaluatedTable RelationshipInstruction::helperHandleTwoProcMaybeWildcard(PqlRelationshipType pqlRsType);
	EvaluatedTable RelationshipInstruction::helperHandleTwoWildcards(PqlRelationshipType pqlRsType);

public:
	RelationshipInstruction(PqlRelationshipType pqlRSType, PqlReference lhs, PqlReference rhs);
	EvaluatedTable execute() override;
};
