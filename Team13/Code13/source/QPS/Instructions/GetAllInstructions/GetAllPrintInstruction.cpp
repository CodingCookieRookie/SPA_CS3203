#include "GetAllPrintInstruction.h"

EvaluatedTable GetAllPrintInstruction::execute() {
	std::vector<StmtIndex> results = Entity::getStmtIdxFromType(StatementType::PRINT_TYPE);
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[synonym] = results;
	return EvaluatedTable(PQLmap);
}

GetAllPrintInstruction::GetAllPrintInstruction(std::string synonym) : GetAllInstruction(synonym) {}
