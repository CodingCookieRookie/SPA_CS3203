#pragma once

#include "RelationshipInstruction.h"
#include "AffectsTProcessor.h"

class AffectsStarInstruction : public RelationshipInstruction {
private:
	EvaluatedTable handleWildCardLeft(std::unordered_map<std::string, std::vector<int>> PQLmap,
		PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices);
	EvaluatedTable handleSynonymLeft(std::unordered_map<std::string, std::vector<int>> PQLmap,
		PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices);
	EvaluatedTable handleIntegerLeft(std::unordered_map<std::string, std::vector<int>> PQLmap,
		PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices);
public:
	AffectsStarInstruction(PqlReference lhsRef, PqlReference rhsRef, AffectsTProcessor* affectsTProcessor, PKBGetter* pkbGetter);
	EvaluatedTable execute() override;
};
