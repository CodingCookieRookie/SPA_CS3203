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
	static std::vector<Instruction*> evaluateToInstructions(ParsedQuery pq);

	/* Helper method to execute all instructions */
	static EvaluatedTable executeInstructions(std::vector<Instruction*> instructions);

	/* Helper method to select columns of table in EvaluatedTable for projection based on Select-cl */
	static EvaluatedTable PQLEvaluator::selectColumnsForProjection(
		EvaluatedTable evaluatedTable,
		std::vector<std::string> columnsProjected,
		std::unordered_map<std::string, PqlEntityType> declarations);

public:
	/* Main entry method for parsing the query */
	static EvaluatedTable evaluate(ParsedQuery& parsedQuery);

	/* Entry method for selecting projected columns from the EvTable */
	static EvaluatedTable PQLEvaluator::selectProjection(EvaluatedTable& resultingEvTable, ParsedQuery& parsedQuery);

};
