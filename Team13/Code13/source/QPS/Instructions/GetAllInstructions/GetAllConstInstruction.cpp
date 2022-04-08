#include "GetAllConstInstruction.h"

EvaluatedTable GetAllConstInstruction::execute() {
	std::vector<int> results = pkbGetter->getAllConsts();
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[synonym] = results;
	return EvaluatedTable(PQLmap);
}

GetAllConstInstruction::GetAllConstInstruction(std::string synonym, PKBGetter* pkbGetter) :
	GetAllInstruction(synonym, pkbGetter) {}
