#pragma once

#include "Instruction.h"

class RelationshipInstruction : public Instruction {
private:
	PqlRelationshipType pqlRelationshipType;
	PqlReference lhsRef;
	PqlReference rhsRef;

	//EvaluatedTable handleModifiesS();
	//EvaluatedTable handleModifiesP();
	//EvaluatedTable handleUsesS();
	//EvaluatedTable handleUsesP();
	EvaluatedTable handleFollows();
	EvaluatedTable handleFollowsT();
	EvaluatedTable handleParent(PqlRelationshipType pqlRsType);
	EvaluatedTable handleParentT();
	EvaluatedTable handleCalls(PqlRelationshipType pqlRsType);
	EvaluatedTable handleNext(PqlRelationshipType pqlRsType);

	/* Helper Handle methods for Modifies-Uses*/
	/*EvaluatedTable handleSynonymLeft(std::unordered_map<std::string, PqlEntityType> PQLentities, std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices, PqlRelationshipType pqlRelationshipType);
	EvaluatedTable handleIntegerLeft(std::unordered_map<std::string, PqlEntityType> PQLentities, std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices);
	EvaluatedTable handleIdentLeft(std::unordered_map<std::string, PqlEntityType> PQLentities, std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices);
	*/
	/* Helper Handle methods */
	EvaluatedTable helperHandleTwoIntegers(PqlRelationshipType pqlRsType);
	EvaluatedTable helperHandleTwoIdents(PqlRelationshipType pqlRsType);
	EvaluatedTable helperHandleOneInt(
		PqlRelationshipType pqlRsType, PqlReferenceType lhsRefType, PqlReferenceType rhsRefType);
	EvaluatedTable helperHandleOneIdent(
		PqlRelationshipType pqlRsType, PqlReferenceType lhsRefType, PqlReferenceType rhsRefType);
	EvaluatedTable helperHandleTwoStmtsMaybeWildcard(PqlRelationshipType pqlRsType);
	EvaluatedTable helperHandleTwoProcMaybeWildcard(PqlRelationshipType pqlRsType);
	EvaluatedTable helperHandleTwoWildcards(PqlRelationshipType pqlRsType);

public:
	RelationshipInstruction();
	RelationshipInstruction(PqlRelationshipType pqlRSType, PqlReference lhs, PqlReference rhs);
	EvaluatedTable execute() override;
	EvaluatedTable helperHandleTwoIntegers();
};
