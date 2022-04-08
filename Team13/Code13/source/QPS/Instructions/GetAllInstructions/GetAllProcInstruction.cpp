#include "GetAllProcInstruction.h"

EvaluatedTable GetAllProcInstruction::execute() {
	std::vector<ProcIndex> results = pkbGetter->getAllNameIdxEntityInfo(EntityType::PROCEDURE);
	std::unordered_map<std::string, std::vector<int>> PQLmap;
	PQLmap[synonym] = results;
	return EvaluatedTable(PQLmap);
}

GetAllProcInstruction::GetAllProcInstruction(std::string synonym, PKBGetter* pkbGetter) :
	GetAllInstruction(synonym, pkbGetter) {}
