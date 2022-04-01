#include "GetAllReadInstruction.h"

EvaluatedTable GetAllReadInstruction::execute() {
	std::vector<StmtIndex> results = Entity::getStmtIdxFromType(StatementType::READ_TYPE);
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[synonym] = results;
	return EvaluatedTable(PQLmap);
}

GetAllReadInstruction::GetAllReadInstruction(std::string synonym) : GetAllInstruction(synonym) {}
