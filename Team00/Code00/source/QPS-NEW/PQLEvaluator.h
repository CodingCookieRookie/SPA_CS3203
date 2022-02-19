#pragma once
#include <string>
#include <vector>

#include "QPSCommons.h"
#include "ParsedQuery.h"
#include "EvaluatedTable.h"
#include "Instruction.h"
#include "../Exception/SPAException.h"

class PQLEvaluator {
private:
	ParsedQuery parsedQuery;

	/* Helper method to break down parsedQuery into isntructions to call in PKB */
	std::vector<Instruction*> evaluateToInstructions(ParsedQuery pq);

	/* Helper method to execute all instructions */
	EvaluatedTable executeInstructions(std::vector<Instruction*> instructions);

public:

	/* Instantiate of a PQLEvaluator */
	PQLEvaluator(ParsedQuery parsedQuery);

	/* Main entry method for parsing the query */
	EvaluatedTable evaluate();
};
