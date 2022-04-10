#include "GetAllPrintInstruction.h"

EvaluatedTable GetAllPrintInstruction::execute() {
	std::vector<StmtIndex> results = pkbGetter->getStmtIdxFromType(StatementType::PRINT_TYPE);
	Table PQLmap;
	PQLmap[synonym] = results;
	return EvaluatedTable(PQLmap);
}

GetAllPrintInstruction::GetAllPrintInstruction(std::string synonym, PKBGetter* pkbGetter) :
	GetAllInstruction(synonym, pkbGetter) {}
