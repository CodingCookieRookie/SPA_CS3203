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
	std::unordered_map<std::string, EntityType> declarations = pq.getDeclarations();
	std::unordered_set<std::string> columns = pq.getColumns();
	std::vector<ParsedRelationship> relationships = pq.getRelationships();
	std::vector<ParsedPattern> patterns = pq.getPatterns();
	std::vector<ParsedWith> withs = pq.getWiths();

	// Assumption: Semantically corrct ParsedQuery
	// 1. Get all relationship results from such-that-clause
	for (ParsedRelationship& relationship : relationships) {
		instructions.push_back(relationship.toInstruction());
		PqlReference lhsRef = relationship.getLhs();
		PqlReference rhsRef = relationship.getRhs();
		PQLEvaluator::insertGetAllInstr(lhsRef, pq, instructions);
		PQLEvaluator::insertGetAllInstr(rhsRef, pq, instructions);
	}

	// 2. Get all pattern results from pattern-clause
	for (size_t i = 0; i < patterns.size(); i++) {
		ParsedPattern parsedPattern = patterns.at(i);
		instructions.push_back(parsedPattern.toInstruction());
	}

	// 3. Get all with results for with-clause
	for (const ParsedWith& with : withs) {
		instructions.push_back(with.toInstruction());
	}

	// TODO: Optimisation: Sort instructions.
	return instructions;
}

void PQLEvaluator::insertGetAllInstr(PqlReference pqlRef, ParsedQuery& pq, std::vector<Instruction*>& instructions) {
	std::unordered_map<std::string, EntityType> declarations = pq.getDeclarations();
	if (isSynonymRef(pqlRef) && pq.isStmtSubtype(pqlRef)) {
		std::string value = pqlRef.second;
		EntityType entType = declarations.at(value);
		ParsedGetAll getAllSynonym = ParsedGetAll(entType, value);
		instructions.push_back(getAllSynonym.toInstruction());
	}
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
	std::unordered_map<std::string, EntityType> declarations = pq.getDeclarations();
	std::unordered_map<std::string, std::vector<int>> table = evaluatedTable.getTableRef();
	std::unordered_map<std::string, std::vector<int>> resultTable;
	EvaluatedTable resultEvTable;

	/* If Select-cl is BOOLEAN */
	ProjectionType projType = ParsedQuery::getProjectionType(attributesProjected);
	if (projType == ProjectionType::BOOLEAN) {
		/* Existence of clauses means clauses determine boolean result,
		=> short circuit and go straight to PQLResultProjector */
		if (ParsedQuery::isClausePresent(pq)) {
			return evaluatedTable;
		} else {
			/* No Clauses, existence of declared synonyms determine boolean result
			=> populate declarations into table */
			for (const std::pair<std::string, EntityType>& synonym : declarations) {
				//EntityType columnType = declarations.at(column);
				ParsedGetAll getAllSynonym = ParsedGetAll(synonym.second, synonym.first);
				Instruction* getAll = getAllSynonym.toInstruction();
				EvaluatedTable evTable = getAll->execute();
				resultEvTable = resultEvTable.innerJoinMerge(evTable);
			}
			return resultEvTable;
		}
	}

	/* For each column that already exists in the final EvTable, take it from the evaluatedTable */
	for (const std::string& column : columnsProjected) {
		if (table.find(column) != table.end()) {
			resultTable[column] = table[column];
		}
	}

	/* If the evaluated table is false or an empty table, use a false table */
	resultEvTable = EvaluatedTable(resultTable);
	if (evaluatedTable.getEvResult() == false
		|| (table.size() > 0 && evaluatedTable.getNumRow() == 0)) {
		resultEvTable = EvaluatedTable(false);
	}

	/* For each column that does not exist in the final EvTable,
	get it via an instruction and then perform a cross product. */
	for (const std::string& column : columnsProjected) {
		if (table.find(column) == table.end()) {
			EntityType columnType = declarations.at(column);
			ParsedGetAll getAllSynonym = ParsedGetAll(columnType, column);
			Instruction* getAll = getAllSynonym.toInstruction();
			EvaluatedTable evTable = getAll->execute();
			resultEvTable = resultEvTable.innerJoinMerge(evTable);
		}
	}
	return resultEvTable;
}
