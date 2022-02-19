#pragma once
#include "EvaluatedTable.h"

class PQLResultProjector
{
private:
	EvaluatedTable evaluatedTable;
	std::vector<std::string> columnsProjected;

public:

	/* Instantiate of a PQLResultProjector */
	PQLResultProjector(EvaluatedTable evTable, std::vector<std::string> columnsProjected);

	/* Todelete: Instantiate of a PQLResultProjector */
	PQLResultProjector();

	/* Resolves the EvaluatedTable results into a list of strings to present the results of the query. */
	std::list<std::string> resolveTableToResults();
};
