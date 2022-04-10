#pragma once

#include "RelationshipInstruction.h"

class UsesInstruction : public RelationshipInstruction {
protected:
	EvaluatedTable handleSynonymLeft(Table PQLmap, PqlReference lhsRef, PqlReference rhsRef,
		std::vector<Index> allStmts, std::vector<Index> varIndices, PqlRelationshipType pqlRelationshipType);
	EvaluatedTable handleIntegerLeft(Table PQLmap, PqlReference lhsRef, PqlReference rhsRef,
		std::vector<Index> allStmts, std::vector<Index> varIndices);
	EvaluatedTable handleIdentLeft(Table PQLmap, PqlReference lhsRef, PqlReference rhsRef,
		std::vector<Index> allStmts, std::vector<Index> varIndices);
	UsesInstruction(PqlReference lhsRef, PqlReference rhsRef, PKBGetter* pkbGetter);
};

class UsesSInstruction : public UsesInstruction {
public:
	UsesSInstruction(PqlReference lhsRef, PqlReference rhsRef, PKBGetter* pkbGetter) : UsesInstruction(lhsRef, rhsRef, pkbGetter) {};
	EvaluatedTable execute() override;
};

class UsesPInstruction : public UsesInstruction {
public:
	UsesPInstruction(PqlReference lhsRef, PqlReference rhsRef, PKBGetter* pkbGetter) : UsesInstruction(lhsRef, rhsRef, pkbGetter) {};
	EvaluatedTable execute() override;
};
