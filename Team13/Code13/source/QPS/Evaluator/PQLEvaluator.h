#pragma once

#include <set>
#include <string>
#include <vector>

#include "../../Exception/SPAException.h"
#include "../../PKB/PKBGetter.h"
#include "../Instructions/GetAllInstructions/GetAllInstruction.h"
#include "../Instructions/Instruction.h"
#include "../Instructions/RelationshipInstruction/RelationshipInstruction.h"
#include "../Instructions/PatternInstruction/PatternInstruction.h"
#include "../Instructions/WithInstruction/WithInstruction.h"
#include "../Parser/ParsedQuery.h"
#include "../QPSCommons.h"
#include "../Processors/AffectsProcessor.h"
#include "../Processors/AffectsTProcessor.h"
#include "../Processors/NextTProcessor.h"
#include "../Processors/Processors.h"

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
	EvaluatedTable executeInstructions(const std::vector<Instruction*>& instructions);

	/* Helper method to select columns of table in EvaluatedTable for projection based on Select-cl */
	EvaluatedTable selectColumnsForProjection(EvaluatedTable evaluatedTable, ParsedQuery& pq);

	/* Helper methods for handling boolean projections */
	EvaluatedTable handleBoolean(EvaluatedTable& evaluatedTable);

	EvaluatedTable& fillInColumns(EvaluatedTable& resultEvTable, EvaluatedTable& evaluatedTable);

	/* Helper methods for handling non-boolean projections, which are synonym and tuple projections */
	EvaluatedTable handleNonBoolean(EvaluatedTable& evaluatedTable);

	Table populateTable(EvaluatedTable& evaluatedTable);

	void insertGetAllInstr(PqlReference pqlRef, ParsedQuery& pq, std::vector<Instruction*>& instructions);

	/* Helper methods for grouping clauses for optimisation */
	std::vector<Instruction*> getBooleanClauses(const std::vector<Instruction*>& instructions);

	std::unordered_map<std::string, std::vector<Instruction*>> buildInstructionGraph(const std::vector<Instruction*>& instructions);

	std::vector<std::pair<int, std::string>> getConnectedComponents(
		const std::unordered_map<std::string, std::vector<Instruction*>>& adjList);

	EvaluatedTable executeBooleanClauses(const std::vector<Instruction*>& instructions);

	EvaluatedTable executeInstructionGraph(
		const std::unordered_map<std::string, std::vector<Instruction*>>& adjList,
		const std::vector<std::pair<int, std::string>>& connectedComponents);

	EvaluatedTable executeConnectedComponent(
		const std::unordered_map<std::string, std::vector<Instruction*>>& adjList,
		const std::string& synonym);

	void cleanupInstructions(std::vector<Instruction*>& instructions);

public:
	PQLEvaluator(ParsedQuery& parsedQuery, PKBGetter* pkbGetter);

	/* Main entry method for parsing the query */
	EvaluatedTable evaluate();

	/* Entry method for selecting projected columns from the EvTable */
	EvaluatedTable selectProjection(EvaluatedTable& resultingEvTable);
};
