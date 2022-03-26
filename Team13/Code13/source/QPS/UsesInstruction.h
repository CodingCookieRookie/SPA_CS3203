#pragma once

#include "RelationShipInstruction.h"

class UsesInstruction : public RelationshipInstruction {
protected:
	PqlReference lhsRef;
	PqlReference rhsRef;
	EvaluatedTable handleSynonymLeft(std::unordered_map<std::string, EntityType> PQLentities, std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices, PqlRelationshipType pqlRelationshipType);
	EvaluatedTable handleIntegerLeft(std::unordered_map<std::string, EntityType> PQLentities, std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices);
	EvaluatedTable handleIdentLeft(std::unordered_map<std::string, EntityType> PQLentities, std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices);
	UsesInstruction(PqlReference lhsRef, PqlReference rhsRef);
};

class UsesSInstruction : public UsesInstruction {
public:
	UsesSInstruction(PqlReference lhsRef, PqlReference rhsRef) : UsesInstruction(lhsRef, rhsRef) {};
	EvaluatedTable execute() override;
};

class UsesPInstruction : public UsesInstruction {
public:
	UsesPInstruction(PqlReference lhsRef, PqlReference rhsRef) : UsesInstruction(lhsRef, rhsRef) {};
	EvaluatedTable execute() override;
};
#pragma once
