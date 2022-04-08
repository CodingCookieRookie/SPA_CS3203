#include "GetAllWhileInstruction.h"

EvaluatedTable GetAllWhileInstruction::execute() {
	std::vector<StmtIndex> results = pkbGetter->getStmtIdxFromType(StatementType::WHILE_TYPE);
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[synonym] = results;
	return EvaluatedTable(PQLmap);
}

GetAllWhileInstruction::GetAllWhileInstruction(std::string synonym, PKBGetter* pkbGetter) :
	GetAllInstruction(synonym, pkbGetter) {}
