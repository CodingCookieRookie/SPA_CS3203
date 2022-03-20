#pragma once

#include "EvaluatedTable.h"
#include "QPSCommons.h"
#include "ParsedQuery.h"

class PQLResultProjector {
private:
	EvaluatedTable evaluatedTable;
	std::vector<std::string> columnsProjected;
	std::unordered_map<std::string, PqlEntityType> declarations;

	static bool isClausePresent(ParsedQuery& parsedQuery);

public:
	/* Resolves the EvaluatedTable results into a list of strings to present the results of the query. */
	static std::list<std::string> PQLResultProjector::resolveTableToResults(
		EvaluatedTable evTable, ParsedQuery& parsedQuery);
};
