#pragma once

#include "RelationshipInstruction.h"

class UsesInstruction : public RelationshipInstruction {
protected:
	EvaluatedTable handleSynonymLeft(std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices, PqlRelationshipType pqlRelationshipType);
	EvaluatedTable handleIntegerLeft(std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices);
	EvaluatedTable handleIdentLeft(std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices);
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
