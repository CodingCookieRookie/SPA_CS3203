#pragma once

#include "RelationshipInstruction.h"
#include "AffectsProcessor.h"

class AffectsInstruction : public RelationshipInstruction {
private:
	EvaluatedTable handleWildCardLeft(std::unordered_map<std::string, std::vector<int>> PQLmap,
		PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices);
	EvaluatedTable handleSynonymLeft(std::unordered_map<std::string, std::vector<int>> PQLmap,
		PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices);
	EvaluatedTable handleIntegerLeft(std::unordered_map<std::string, std::vector<int>> PQLmap,
		PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices);

public:
	AffectsInstruction(PqlReference lhsRef, PqlReference rhsRef, AffectsProcessor* affectsProcessor, PKBGetter* pkbGetter);
	EvaluatedTable execute() override;
	QueryScore getDifficultyScore() override;
};
