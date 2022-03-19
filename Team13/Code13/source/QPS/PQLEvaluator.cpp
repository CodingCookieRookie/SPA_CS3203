#include <string>
#include <unordered_map>
#include <unordered_set>

#include "../PKB/Entity.h"
#include "Instruction.h"
#include "PQLEvaluator.h"
#include "QPSCommons.h"

EvaluatedTable PQLEvaluator::evaluate(ParsedQuery& parsedQuery) {
	std::vector<Instruction*> instructions = PQLEvaluator::evaluateToInstructions(parsedQuery);
	EvaluatedTable resultingEvTable = PQLEvaluator::executeInstructions(instructions);
	return resultingEvTable;
}

EvaluatedTable PQLEvaluator::selectProjection(EvaluatedTable& resultingEvTable, ParsedQuery& parsedQuery) {
	EvaluatedTable projectingEvTable = PQLEvaluator::selectColumnsForProjection(resultingEvTable, parsedQuery);
	return projectingEvTable;
}

std::vector<Instruction*> PQLEvaluator::evaluateToInstructions(ParsedQuery pq) {
	std::vector<Instruction*> instructions = std::vector<Instruction*>();
	std::unordered_map<std::string, PqlEntityType> declarations = pq.getDeclarations();
	std::unordered_set<std::string> columns = pq.getColumns();
	std::vector<ParsedRelationship> relationships = pq.getRelationships();
	std::vector<ParsedPattern> patterns = pq.getPatterns();
	std::vector<ParsedWith> withs = pq.getWiths();

	// Assumption: Semantically corrct ParsedQuery
	// 1. Get all relationship results from such-that-clause
	for (size_t i = 0; i < relationships.size(); i++) {
		ParsedRelationship parsedRelationship = relationships.at(i);
		PqlReference lhsRef = parsedRelationship.getLhs();
		PqlReference rhsRef = parsedRelationship.getRhs();
		instructions.push_back(new RelationshipInstruction(parsedRelationship.getRelationshipType(), lhsRef, rhsRef));
		if (isSynonymRef(lhsRef) && pq.isStmtSubtype(lhsRef)) {
			std::string lhsVal = lhsRef.second;
			PqlEntityType lhsType = declarations.at(lhsVal);
			instructions.push_back(new GetAllInstruction(lhsType, lhsVal));
		}
		if (isSynonymRef(rhsRef) && pq.isStmtSubtype(rhsRef)) {
			std::string rhsVal = rhsRef.second;
			PqlEntityType rhsType = declarations.at(rhsVal);
			instructions.push_back(new GetAllInstruction(rhsType, rhsVal));
		}
	}

	// 2. Get all pattern results from pattern-clause
	for (size_t i = 0; i < patterns.size(); i++) {
		ParsedPattern parsedPattern = patterns.at(i);
		instructions.push_back(new PatternInstruction(parsedPattern.getSynonym(), parsedPattern.getPatternType(), parsedPattern.getEntRef(), parsedPattern.getExpression()));
	}

	// 3. Get all with results for with-clause
	for (const ParsedWith& with : withs) {
		PqlReference lhs = with.getLhs();
		PqlReference rhs = with.getRhs();
		PqlEntityType lhsEntity = declarations.find(lhs.second) != declarations.end() ? declarations.at(lhs.second) : PqlEntityType::Constant;
		PqlEntityType rhsEntity = declarations.find(rhs.second) != declarations.end() ? declarations.at(rhs.second) : PqlEntityType::Constant;
		instructions.push_back(new WithInstruction(lhs, rhs, lhsEntity, rhsEntity, with.getAttribType()));
	}

	// TODO: Optimisation: Sort instructions.
	return instructions;

}

EvaluatedTable PQLEvaluator::executeInstructions(std::vector<Instruction*> instructions) {
	EvaluatedTable resultEvTable;
	std::unordered_set<std::string> currentTableColumns;
	for (size_t i = 0; i < instructions.size(); i++) {
		Instruction* instruction = instructions.at(i);
		EvaluatedTable evTable = instruction->execute();
		resultEvTable = resultEvTable.innerJoinMerge(evTable);
	}
	return resultEvTable;
}

EvaluatedTable PQLEvaluator::selectColumnsForProjection(
	EvaluatedTable evaluatedTable, ParsedQuery& pq) {
	std::unordered_set<std::string> columnsProjected = pq.getColumns();
	std::vector<PqlReference> attributesProjected = pq.getAttributes();
	std::unordered_map<std::string, PqlEntityType> declarations = pq.getDeclarations();
	std::unordered_map<std::string, std::vector<int>> table = evaluatedTable.getTableRef();
	std::unordered_map<std::string, PqlEntityType> resultEntities;
	std::unordered_map<std::string, std::vector<int>> resultTable;
	EvaluatedTable resultEvTable;

	/* For each column that already exists in the final EvTable, take it from the evaluatedTable */
	for (const std::string& column : columnsProjected) {
		if (table.find(column) != table.end()) {
			resultEntities[column] = declarations[column];
			resultTable[column] = table[column];
		}
	}

	/* If the evaluated table is false or an empty table, use a false table */
	resultEvTable = EvaluatedTable(resultEntities, resultTable);
	if (evaluatedTable.getEvResult() == false
		|| (table.size() > 0 && evaluatedTable.getNumRow() == 0)) {
		resultEvTable = EvaluatedTable(false);
	}

	/* For each column that does not exist in the final EvTable,
	get it via an instruction and then perform a cross product. */
	for (const std::string& column : columnsProjected) {
		if (table.find(column) == table.end()) {
			PqlEntityType columnType = declarations.at(column);
			Instruction* getAll = new GetAllInstruction(columnType, column);
			EvaluatedTable evTable = getAll->execute();
			resultEvTable = resultEvTable.innerJoinMerge(evTable);
		}
	}
	return resultEvTable;
}
