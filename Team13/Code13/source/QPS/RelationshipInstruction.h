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
	EvaluatedTable handleParent();
	EvaluatedTable handleParentT();
	EvaluatedTable handleCalls(std::string PqlRsType);

	/* Helper Handle methods */
	EvaluatedTable helperHandleTwoIdents(std::string pqlRsType);
	EvaluatedTable RelationshipInstruction::helperHandleOneIdent(
		std::string pqlRsType, PqlReferenceType lhsRefType, PqlReferenceType rhsRefType);
	EvaluatedTable RelationshipInstruction::helperHandleTwoProcMaybeWildcard(std::string pqlRsType);
	EvaluatedTable RelationshipInstruction::helperHandleTwoWildcards(std::string pqlRsType);

public:
	RelationshipInstruction(PqlRelationshipType pqlRSType, PqlReference lhs, PqlReference rhs);
	EvaluatedTable execute() override;
};
