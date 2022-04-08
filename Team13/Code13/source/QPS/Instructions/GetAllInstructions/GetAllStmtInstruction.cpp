#include "GetAllStmtInstruction.h"

EvaluatedTable GetAllStmtInstruction::execute() {
	std::vector<StmtIndex> results = pkbGetter->getAllStmts();
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[synonym] = results;
	return EvaluatedTable(PQLmap);
}

GetAllStmtInstruction::GetAllStmtInstruction(std::string synonym, PKBGetter* pkbGetter) :
	GetAllInstruction(synonym, pkbGetter) {}
