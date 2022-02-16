#pragma once
#include <string>
#include <vector>

#include "QPSCommons.h"
#include "ParsedQuery.h"
#include "EvaluatedTable.h"
#include "Instruction.h"


class PQLEvaluator {
private:
	ParsedQuery parsedQuery;

	/* Helper method to break down parsedQuery into isntructions to call in PKB */
	std::vector<Instruction*> evaluateToInstructions(ParsedQuery pq);

	/* Helper method to execute all instructions */
	EvaluatedTable executeInstructions(std::vector<Instruction*> instructions);

	/* Helper method to execute one instruction */
	EvaluatedTable execute(Instruction instr);

	/* Helper method to inner join two EvaluatedTables together via a hash join algo */
	// void innerJoinMerge(EvaluatedTable& currentTable, EvaluatedTable& newTable);

public:

	/* Instantiate of a PQLEvaluator */
	PQLEvaluator(ParsedQuery parsedQuery);

	/* Main entry method for parsing the query */
	EvaluatedTable evaluate();

	/* Merges the current table with values from another EvaluatedTable
	via an Inner Join, which can be implemented using Hash Join */
	EvaluatedTable innerJoinMerge(EvaluatedTable evTable, EvaluatedTable newEvTable, std::unordered_set<std::string> currentTableColumns);

	///* Merges the current table with values from another EvaluatedTable
	//via an Inner Join, which can be implemented using Hash Join */
	//EvaluatedTable innerJoinMerge(EvaluatedTable evTable, EvaluatedTable newEvTable, std::string stmtRef, std::string entRef, std::unordered_set<std::string> currentTableColumns);

};
