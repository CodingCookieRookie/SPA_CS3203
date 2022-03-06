#pragma once

#include <string>
#include <vector>

#include "../Exception/SPAException.h"
#include "Instruction.h"
#include "ParsedQuery.h"
#include "QPSCommons.h"

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
