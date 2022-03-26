#pragma once

#include "RelationShipInstruction.h"

class ModifiesInstruction : public RelationshipInstruction {
protected:
	PqlReference lhsRef;
	PqlReference rhsRef;
	EvaluatedTable handleSynonymLeft(std::unordered_map<std::string, EntityType> PQLentities, std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices, PqlRelationshipType pqlRelationshipType);
	EvaluatedTable handleIntegerLeft(std::unordered_map<std::string, EntityType> PQLentities, std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices);
	EvaluatedTable handleIdentLeft(std::unordered_map<std::string, EntityType> PQLentities, std::unordered_map<std::string, std::vector<int>> PQLmap, PqlReference lhsRef, PqlReference rhsRef, std::vector<int> allStmts, std::vector<int> varIndices);
	ModifiesInstruction(PqlReference lhsRef, PqlReference rhsRef);
};

class ModifiesSInstruction : public ModifiesInstruction {
public:
	ModifiesSInstruction(PqlReference lhsRef, PqlReference rhsRef) : ModifiesInstruction(lhsRef, rhsRef) {};
	EvaluatedTable execute() override;
};

class ModifiesPInstruction : public ModifiesInstruction {
public:
	ModifiesPInstruction(PqlReference lhsRef, PqlReference rhsRef) : ModifiesInstruction(lhsRef, rhsRef) {};
	EvaluatedTable execute() override;
};
