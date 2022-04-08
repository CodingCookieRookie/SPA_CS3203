#include "GetAllVarInstruction.h"

EvaluatedTable GetAllVarInstruction::execute() {
	std::vector<VarIndex> results = pkbGetter->getAllNameIdxEntityInfo(EntityType::VARIABLE);
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[synonym] = results;
	return EvaluatedTable(PQLmap);
}

GetAllVarInstruction::GetAllVarInstruction(std::string synonym, PKBGetter* pkbGetter) :
	GetAllInstruction(synonym, pkbGetter) {}
