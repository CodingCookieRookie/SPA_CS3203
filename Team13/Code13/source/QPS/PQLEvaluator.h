#pragma once

#include <string>
#include <vector>

#include "../Exception/SPAException.h"
#include "../PKB/PKBGetter.h"
#include "../QPS/Instructions/GetAllInstructions/GetAllInstruction.h"
#include "Instruction.h"
#include "RelationshipInstruction.h"
#include "PatternInstruction.h"
#include "WithInstruction.h"
#include "ParsedQuery.h"
#include "QPSCommons.h"
#include "AffectsProcessor.h"
#include "AffectsTProcessor.h"
#include "NextTProcessor.h"
#include "Processors.h"

class PQLEvaluator {
private:
	ParsedQuery& parsedQuery;
	PKBGetter* pkbGetter;
	Processors processors;

	/* Helper method to instantiate the 3 processors for processing N* / A / A* relationships */
	Processors instantiateProcessors();

	/* Helper method to break down parsedQuery into instructions to call in PKB */
	std::vector<Instruction*> evaluateToInstructions(ParsedQuery pq);

	/* Helper method to execute all instructions */
	EvaluatedTable executeInstructions(std::vector<Instruction*> instructions);

	/* Helper method to select columns of table in EvaluatedTable for projection based on Select-cl */
	EvaluatedTable selectColumnsForProjection(EvaluatedTable evaluatedTable, ParsedQuery& pq);

	void insertGetAllInstr(PqlReference pqlRef, ParsedQuery& pq, std::vector<Instruction*>& instructions);

public:
	/* Constructor */
	PQLEvaluator(ParsedQuery& parsedQuery, PKBGetter* pkbGetter);

	/* Main entry method for parsing the query */
	EvaluatedTable evaluate();

	/* Entry method for selecting projected columns from the EvTable */
	EvaluatedTable selectProjection(EvaluatedTable& resultingEvTable);
};
