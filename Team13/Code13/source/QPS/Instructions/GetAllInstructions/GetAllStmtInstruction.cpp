#include "GetAllStmtInstruction.h"

EvaluatedTable GetAllStmtInstruction::execute() {
	std::vector<StmtIndex> results = pkbGetter->getAllStmts();
	Table PQLmap;
	PQLmap[synonym] = results;
	return EvaluatedTable(PQLmap);
}

GetAllStmtInstruction::GetAllStmtInstruction(std::string synonym, PKBGetter* pkbGetter) :
	GetAllInstruction(synonym, pkbGetter) {}
