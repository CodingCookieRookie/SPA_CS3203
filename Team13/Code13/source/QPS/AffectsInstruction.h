#pragma once

#include "RelationshipInstruction.h"
#include "AffectsProcessor.h"

class AffectsInstruction : public RelationshipInstruction {
private:
	AffectsProcessor* affectsProcessor;
	EvaluatedTable handleWildCardLeft(std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices);
	EvaluatedTable handleSynonymLeft(std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices);
	EvaluatedTable handleIntegerLeft(std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices);

public:
	AffectsInstruction(PqlReference lhsRef, PqlReference rhsRef);
	AffectsInstruction(PqlReference lhsRef, PqlReference rhsRef, AffectsProcessor* affectsProcessor) {
		this->lhsRef = lhsRef;
		this->rhsRef = rhsRef;
		this->affectsProcessor = affectsProcessor;
	}
	EvaluatedTable execute() override;
};
