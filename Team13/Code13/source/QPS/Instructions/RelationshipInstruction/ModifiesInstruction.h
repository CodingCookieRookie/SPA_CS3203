#pragma once

#include "RelationshipInstruction.h"

class ModifiesInstruction : public RelationshipInstruction {
protected:
	EvaluatedTable handleSynonymLeft(Table PQLmap, PqlReference lhsRef, PqlReference rhsRef,
		std::vector<Index> allStmts, std::vector<Index> varIndices, PqlRelationshipType pqlRelationshipType);
	EvaluatedTable handleIntegerLeft(Table PQLmap, PqlReference lhsRef, PqlReference rhsRef,
		std::vector<Index> allStmts, std::vector<Index> varIndices);
	EvaluatedTable handleIdentLeft(Table PQLmap, PqlReference lhsRef, PqlReference rhsRef,
		std::vector<Index> allStmts, std::vector<Index> varIndices);
	ModifiesInstruction(PqlReference lhsRef, PqlReference rhsRef, PKBGetter* pkbGetter);
};

class ModifiesSInstruction : public ModifiesInstruction {
public:
	ModifiesSInstruction(PqlReference lhsRef, PqlReference rhsRef, PKBGetter* pkbGetter) : ModifiesInstruction(lhsRef, rhsRef, pkbGetter) {};
	EvaluatedTable execute() override;
};

class ModifiesPInstruction : public ModifiesInstruction {
public:
	ModifiesPInstruction(PqlReference lhsRef, PqlReference rhsRef, PKBGetter* pkbGetter) : ModifiesInstruction(lhsRef, rhsRef, pkbGetter) {};
	EvaluatedTable execute() override;
};
