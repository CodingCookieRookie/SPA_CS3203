#include "PQLEvaluator.h"

#include <string>
#include <optional>

EvaluatedTable evaluate(ParsedQuery parsedQuery) {
	
	std::vector<INSTRUCTION> instructions = evalauteToInstructions(parsedQuery);
	
	EvaluatedTable resultEvTable = EvaluatedTable();
	for (INSTRUCTION instruction : instructions) {
		EvaluatedTable currEvTable = executeInstruction(instruction);
		if !(resultEvTable.entities) {
			resultEvTable = currEvTable;
		} else {
			resultEvTable.innerJoinMerge(currEvTable);
		}
	}
	return resultEvTable;
}

std::vector<INSTRUCTION> evalauteToInstructions(ParsedQuery& pq) {
	std::vector<INSTRUCTION> instructions;
	// 1. Checks for what is required in the Select clause
	std::vector<std::string> columns = pq.getColumns();
	for (std::string entity : columns) {
		// Match entity to 
	}
	// 2. If there are relations
	//		checks for what relations are included in the ParsedQuery
	//		Add respective instructions
	// 3. If there are patterns
	//		checks for what patterns are included in the ParsedQuery
	//		Add respective instructions

	//TODO: Optimisation: Sort instructions.
}

EvaluatedTable executeInstruction(INSTRUCTION instruction) {
	EvaluatedTable PKBresults = EvaluatedTable();
	// Call relevant API

	return PKBresults;
}

void innerJoinMerge(EvaluatedTable& newEvTable) {
	// 1. Short-circuit if other table is empty
	// 2. 
}
