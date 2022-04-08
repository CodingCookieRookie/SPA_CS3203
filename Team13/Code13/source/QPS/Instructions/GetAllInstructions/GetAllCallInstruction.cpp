#include "GetAllCallInstruction.h"

EvaluatedTable GetAllCallInstruction::execute() {
	std::vector<StmtIndex> results = pkbGetter->getStmtIdxFromType(StatementType::CALL_TYPE);
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[synonym] = results;
	return EvaluatedTable(PQLmap);
}

GetAllCallInstruction::GetAllCallInstruction(std::string synonym, PKBGetter* pkbGetter) :
	GetAllInstruction(synonym, pkbGetter) {}
