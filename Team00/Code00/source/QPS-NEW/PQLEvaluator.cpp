#include "PQLEvaluator.h"
#include "QPSCommons.h"
#include "Instruction.h"

#include <string>
#include <optional>


EvaluatedTable PQLEvaluator::evaluate(ParsedQuery parsedQuery) {
	std::vector<Instruction> instructions = PQLEvaluator::evaluateToInstructions(parsedQuery);
	EvaluatedTable resultingEvTable = executeInstructions(instructions);
	return resultingEvTable;
}


std::vector<Instruction> PQLEvaluator::evaluateToInstructions(ParsedQuery& pq) {
	std::vector<Instruction> instructions = std::vector<Instruction>();
	std::unordered_map<std::string, PQL_VARIABLE_TYPE> declarations = pq.getDeclarations();
	std::vector<std::string> columns = pq.getColumns();
	
	// 1. Check if entitiy in Select clause is found in declarations
	for (std::string entity : columns) {
		PQL_VARIABLE_TYPE entityTypeRequired = declarations.at(entity);

		switch (entityTypeRequired) {
		case PQL_VARIABLE_TYPE::STMT :
			instructions.push_back(Instruction(INSTRUCTION_TYPE::getAllStmt));
			break;
		case PQL_VARIABLE_TYPE::ASSIGN:
			instructions.push_back(Instruction(INSTRUCTION_TYPE::getAllAsgn));
			break;
		}
	}

	// TODO:
	// 2. If there are relations
	//		checks for what relations are included in the ParsedQuery
	//		Add respective instructions
	// 3. If there are patterns
	//		checks for what patterns are included in the ParsedQuery
	//		Add respective instructions

	// TODO: Optimisation: Sort instructions.
	return instructions;
}

EvaluatedTable PQLEvaluator::executeInstructions(std::vector<Instruction> instructions) {
	EvaluatedTable resultEvTable = EvaluatedTable();

	// Call relevant API
	for (Instruction instruction : instructions) {
		EvaluatedTable currEvTable = execute(instruction);
		if (resultEvTable.getEntities().empty()) {
			resultEvTable = currEvTable;
		}
		else {
			// TODO: Merge Tables
			// resultEvTable = innerJoinMerge(resultEvTable, currEvTable);
		}
	}

	return resultEvTable;
}

EvaluatedTable PQLEvaluator::execute(Instruction& instr) {
	INSTRUCTION_TYPE instrType = instr.getType();
	EvaluatedTable currTable;

	switch (instrType) {

	case INSTRUCTION_TYPE::getAllStmt:
		//PKB's getAllStmts
		//currTable = getAllStmts();
		break;
	}

	return currTable;
}

EvaluatedTable PQLEvaluator::innerJoinMerge(EvaluatedTable& evTable, EvaluatedTable& newEvTable) {
	EvaluatedTable mergedTable;
	// TODO:
	// 1. Short-circuit if other table is empty
	// 2.

	mergedTable = evTable;
	return mergedTable;
}
