#pragma once

#include "RelationshipInstruction.h"

class ModifiesInstruction : public RelationshipInstruction {
protected:
	EvaluatedTable handleSynonymLeft(std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices, PqlRelationshipType pqlRelationshipType);
	EvaluatedTable handleIntegerLeft(std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices);
	EvaluatedTable handleIdentLeft(std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices);
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
