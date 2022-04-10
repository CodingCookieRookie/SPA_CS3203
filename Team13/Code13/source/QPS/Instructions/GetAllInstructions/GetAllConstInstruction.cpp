#include "GetAllConstInstruction.h"

EvaluatedTable GetAllConstInstruction::execute() {
	std::vector<int> results = pkbGetter->getAllConsts();
	Table PQLmap;
	PQLmap[synonym] = results;
	return EvaluatedTable(PQLmap);
}

GetAllConstInstruction::GetAllConstInstruction(std::string synonym, PKBGetter* pkbGetter) :
	GetAllInstruction(synonym, pkbGetter) {}
