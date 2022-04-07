#pragma once

#include <string>
#include <vector>

#include "../Exception/SPAException.h"
#include "Instruction.h"
#include "Instructions/GetAllInstructions/GetAllInstruction.h"
#include "RelationshipInstruction.h"
#include "PatternInstruction.h"
#include "WithInstruction.h"
#include "ParsedQuery.h"
#include "QPSCommons.h"
#include "Processors.h"

class PQLEvaluator {
private:
	ParsedQuery& parsedQuery;
	Processors processors;

	/* Helper method to break down parsedQuery into instructions to call in PKB */
	std::vector<Instruction*> evaluateToInstructions();

	/* Helper method to break down to insert GetAll instructions to call in PKB */
	void insertGetAllInstr(PqlReference pqlRef, std::vector<Instruction*>& instructions);

	/* Helper method to execute all instructions */
	EvaluatedTable executeInstructions(std::vector<Instruction*> instructions);

	/* Helper method to select columns of table in EvaluatedTable for projection based on Select-cl */
	EvaluatedTable selectColumnsForProjection(EvaluatedTable& evaluatedTable);

	/* Helper to instantiate processors */
	Processors instantiateProcessors();

public:
	/* Constructor */
	PQLEvaluator(ParsedQuery& parsedQuery);

	/* Main entry method for parsing the query */
	EvaluatedTable evaluate();

	/* Entry method for selecting projected columns from the EvTable */
	EvaluatedTable selectProjection(EvaluatedTable& resultingEvTable);
};
