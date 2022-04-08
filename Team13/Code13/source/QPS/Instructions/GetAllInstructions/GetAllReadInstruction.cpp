#include "GetAllReadInstruction.h"

EvaluatedTable GetAllReadInstruction::execute() {
	std::vector<StmtIndex> results = pkbGetter->getStmtIdxFromType(StatementType::READ_TYPE);
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[synonym] = results;
	return EvaluatedTable(PQLmap);
}

GetAllReadInstruction::GetAllReadInstruction(std::string synonym, PKBGetter* pkbGetter) :
	GetAllInstruction(synonym, pkbGetter) {}
