#include "PQLEvaluator.h"
#include "QPSCommons.h"
#include "Instruction.h"

#include <string>
#include <optional>


EvaluatedTable PQLEvaluator::evaluate(ParsedQuery parsedQuery) {
	
	std::vector<Instruction> instructions = PQLEvaluator::evaluateToInstructions(parsedQuery);
	
	EvaluatedTable resultEvTable = EvaluatedTable();
	for (Instruction instruction : instructions) {
		EvaluatedTable currEvTable = executeInstruction(instruction);
		if (resultEvTable.getEntities().empty()) {
			resultEvTable = currEvTable;
		} else {
			resultEvTable = innerJoinMerge(resultEvTable, currEvTable);
		}
	}
	return resultEvTable;
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

	// 2. If there are relations
	//		checks for what relations are included in the ParsedQuery
	//		Add respective instructions
	// 3. If there are patterns
	//		checks for what patterns are included in the ParsedQuery
	//		Add respective instructions

	//TODO: Optimisation: Sort instructions.
	return instructions;
}

EvaluatedTable executeInstruction(Instruction instruction) {
	EvaluatedTable PKBresults = EvaluatedTable();
	// Call relevant API

	return PKBresults;
}

EvaluatedTable PQLEvaluator::innerJoinMerge(EvaluatedTable& evTable, EvaluatedTable& newEvTable) {
	// 1. Short-circuit if other table is empty
	// 2.
}
