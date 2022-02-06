#pragma once
#include <string>
#include <vector>

#include "QPSCommons.h"
#include "ParsedQuery.h"
#include "EvaluatedTable.h"


class PQLEvaluator {
private:
	/* Helper method to break down parsedQuery into isntructions to call in PKB*/
	std::vector<INSTRUCTIONS> evalauteToInstructions(ParsedQuery parsedQuery);

public:

	/* Instantiate of a PQLEvaluator */
	PQLEvaluator(ParsedQuery parsedQuery);

	/* Main entry method for parsing the query */
	EvaluatedTable evaluate();

	/* Merges the current table with values from another EvaluatedTable
	via an Inner Join, which can be implemented using Hash Join */
	void innerJoinMerge(EvaluatedTable& anotherEvaluatedTable);
};

enum class INSTRUCTION {
	getVar, getAsgn
};

