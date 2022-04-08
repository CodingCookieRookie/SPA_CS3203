#pragma once

#include "EvaluatedTable.h"
#include "QPSCommons.h"
#include "ParsedQuery.h"
#include "../PKB/PKBGetter.h"

class PQLResultProjector {
private:
	EvaluatedTable& evTable;
	ParsedQuery& parsedQuery;
	PKBGetter* pkbGetter;
	std::vector<std::string> columnsProjected;
	std::unordered_map<std::string, EntityType> declarations;

public:
	PQLResultProjector(EvaluatedTable& evTable, ParsedQuery& parsedQuery, PKBGetter* pkbGetter);

	/* Resolves the EvaluatedTable results into a list of strings to present the results of the query. */
	std::list<std::string> PQLResultProjector::resolveTableToResults();
};
