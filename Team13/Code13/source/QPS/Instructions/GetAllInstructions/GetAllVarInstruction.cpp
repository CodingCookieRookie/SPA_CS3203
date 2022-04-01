#include "GetAllVarInstruction.h"

EvaluatedTable GetAllVarInstruction::execute() {
	std::vector<VarIndex> results = Entity::getAllVars();
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[synonym] = results;
	return EvaluatedTable(PQLmap);
}

GetAllVarInstruction::GetAllVarInstruction(std::string synonym) : GetAllInstruction(synonym) {}
