#pragma once

#include "RelationshipInstruction.h"
#include "AffectsTProcessor.h"

class AffectsTInstruction : public RelationshipInstruction {
private:
	static const int AFFECTS_T_DIFFICULTY;
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
	AffectsTInstruction(PqlReference lhsRef, PqlReference rhsRef, AffectsTProcessor* affectsTProcessor, PKBGetter* pkbGetter);
	EvaluatedTable execute() override;
	QueryScore getDifficultyScore() override;
};
