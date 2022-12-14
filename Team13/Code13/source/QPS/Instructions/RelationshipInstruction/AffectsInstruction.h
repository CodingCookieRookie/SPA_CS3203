#pragma once

#include "RelationshipInstruction.h"
#include "../../Processors/AffectsProcessor.h"

class AffectsInstruction : public RelationshipInstruction {
private:
	static const int AFFECTS_DIFFICULTY;
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
