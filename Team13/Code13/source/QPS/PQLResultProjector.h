#pragma once

#include "EvaluatedTable.h"
#include "QPSCommons.h"
#include "ParsedQuery.h"

class PQLResultProjector {
private:
	EvaluatedTable evaluatedTable;
	ParsedQuery& parsedQuery;

public:
	PQLResultProjector(EvaluatedTable& evaluatedTable, ParsedQuery& parsedQuery);

	/* Resolves the EvaluatedTable results into a list of strings to present the results of the query. */
	std::list<std::string> PQLResultProjector::resolveTableToResults();
};
