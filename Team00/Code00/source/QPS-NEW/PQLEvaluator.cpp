#include "PQLEvaluator.h"

EvaluatedTable evaluate(ParsedQuery parsedQuery) {
	// Question: Does the semantic check happen here?
	
	std::vector<INSTRUCTION> instructions = evalauteToInstructions(parsedQuery);
	
	std::vector<EvaluatedTable> evaluatedTables;
	for (INSTRUCTION instruction : instructions) {
		evaluatedTables.push_back(executeInstruction(instruction));
	}
	//mergeInstructions(evaluatedTables);

}

std::vector<INSTRUCTION> evalauteToInstructions(ParsedQuery parsedQuery) {
	// 1. Checks for what is required in the Select clause
	//		Add respective instructions
	// 2. If there are relations
	//		checks for what relations are included in the ParsedQuery
	//		Add respective instructions
	// 3. If there are patterns
	//		checks for what patterns are included in the ParsedQuery
	//		Add respective instructions

	//TODO: Optimisation: Sort instructions.
}

EvaluatedTable executeInstruction(INSTRUCTION instruction) {

}
