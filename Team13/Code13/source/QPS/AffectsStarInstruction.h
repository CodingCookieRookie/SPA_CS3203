#pragma once

#include "RelationshipInstruction.h"
#include "AffectsTProcessor.h"

class AffectsStarInstruction : public RelationshipInstruction {
private:
	AffectsProcessor* affectsProcessor;
	AffectsTProcessor* affectsTProcessor;
	EvaluatedTable handleWildCardLeft(std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices);
	EvaluatedTable handleSynonymLeft(std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices);
	EvaluatedTable handleIntegerLeft(std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices);
public:
	AffectsStarInstruction(PqlReference lhsRef, PqlReference rhsRef) : RelationshipInstruction(lhsRef, rhsRef) {};
	AffectsStarInstruction(PqlReference lhsRef, PqlReference rhsRef, AffectsProcessor* affectsProcessor, AffectsTProcessor* affectsTProcessor) {
		this->lhsRef = lhsRef;
		this->rhsRef = rhsRef;
		this->affectsProcessor = affectsProcessor;
		this->affectsTProcessor = affectsTProcessor;
	}
	EvaluatedTable execute() override;
};
