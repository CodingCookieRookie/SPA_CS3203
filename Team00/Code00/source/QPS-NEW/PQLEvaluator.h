#pragma once
#include <string>
#include <vector>

#include "QPSCommons.h"
#include "ParsedQuery.h"
#include "EvaluatedTable.h"


class PQLEvaluator {
private:
	std::vector<PQL_VARIABLE> parseSingleDeclaration();

public:

	/* @brief Instiation of a PQLEvaluator */
	PQLEvaluator(ParsedQuery parsedQuery);

	/* @brief Main entry method for parsing the query */
	EvaluatedTable evaluate();

	/* Merges the current table with values from another EvaluatedTable
	via an Inner Join, which can be implemented using Hash Join */
	void innerJoinMerge(EvaluatedTable& anotherEvaluatedTable);
};

