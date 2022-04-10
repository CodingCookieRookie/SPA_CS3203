#pragma once

#include "RelationshipInstruction.h"
#include "AffectsProcessor.h"

class AffectsInstruction : public RelationshipInstruction {
private:
	EvaluatedTable handleWildCardLeft(Table PQLmap,
		PqlReference lhsRef, PqlReference rhsRef,
		std::vector<Index> allStmts, std::vector<Index> varIndices);
	EvaluatedTable handleSynonymLeft(Table PQLmap,
		PqlReference lhsRef, PqlReference rhsRef,
		std::vector<Index> allStmts, std::vector<Index> varIndices);
	EvaluatedTable handleIntegerLeft(Table PQLmap,
		PqlReference lhsRef, PqlReference rhsRef,
		std::vector<Index> allStmts, std::vector<Index> varIndices);

public:
	AffectsInstruction(PqlReference lhsRef, PqlReference rhsRef,
		AffectsProcessor* affectsProcessor, PKBGetter* pkbGetter);
	EvaluatedTable execute() override;
	QueryScore getDifficultyScore() override;
};
