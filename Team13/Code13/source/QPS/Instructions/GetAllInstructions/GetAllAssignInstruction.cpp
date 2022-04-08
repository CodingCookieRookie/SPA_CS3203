#include "GetAllAssignInstruction.h"

EvaluatedTable GetAllAssignInstruction::execute() {
	std::vector<StmtIndex> results = pkbGetter->getStmtIdxFromType(StatementType::ASSIGN_TYPE);
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[synonym] = results;
	return EvaluatedTable(PQLmap);
}

GetAllAssignInstruction::GetAllAssignInstruction(std::string synonym, PKBGetter* pkbGetter) :
	GetAllInstruction(synonym, pkbGetter) {}
