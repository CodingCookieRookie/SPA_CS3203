#include "GetAllIfInstruction.h"

EvaluatedTable GetAllIfInstruction::execute() {
	std::vector<StmtIndex> results = pkbGetter->getStmtIdxFromType(StatementType::IF_TYPE);
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[synonym] = results;
	return EvaluatedTable(PQLmap);
}

GetAllIfInstruction::GetAllIfInstruction(std::string synonym, PKBGetter* pkbGetter) :
	GetAllInstruction(synonym, pkbGetter) {}
