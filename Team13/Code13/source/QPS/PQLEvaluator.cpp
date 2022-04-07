#include <string>
#include <unordered_map>
#include <unordered_set>

#include "../PKB/Entity.h"
#include "PQLEvaluator.h"

Processors PQLEvaluator::instantiateProcessors() {
	NextTProcessor* nextTProcessor = new NextTProcessor(new NextTCache());
	AffectsProcessor* affectsProcessor = new AffectsProcessor();
	AffectsTProcessor* affectsTProcessor = new AffectsTProcessor(new AffectsTCache(), affectsProcessor);
	Processors processors = Processors(nextTProcessor, affectsProcessor, affectsTProcessor);
	return processors;
}

PQLEvaluator::PQLEvaluator(ParsedQuery& parsedQuery) :
	parsedQuery(parsedQuery), processors(PQLEvaluator::instantiateProcessors()) {}

EvaluatedTable PQLEvaluator::evaluate() {
	std::vector<Instruction*> instructions = PQLEvaluator::evaluateToInstructions();
	EvaluatedTable resultingEvTable = PQLEvaluator::executeInstructions(instructions);
	return resultingEvTable;
}

EvaluatedTable PQLEvaluator::selectProjection(EvaluatedTable& resultingEvTable) {
	EvaluatedTable projectingEvTable = PQLEvaluator::selectColumnsForProjection(resultingEvTable);
	return projectingEvTable;
}

std::vector<Instruction*> PQLEvaluator::evaluateToInstructions() {
	std::vector<Instruction*> instructions = std::vector<Instruction*>();
	std::unordered_map<std::string, EntityType> declarations = parsedQuery.getDeclarations();
	std::unordered_set<std::string> columns = parsedQuery.getColumns();
	std::vector<ParsedRelationship> relationships = parsedQuery.getRelationships();
	std::vector<ParsedPattern> patterns = parsedQuery.getPatterns();
	std::vector<ParsedWith> withs = parsedQuery.getWiths();

	// Assumption: Semantically corrct ParsedQuery
	// 1. Get all relationship results from such-that-clause
	for (ParsedRelationship& relationship : relationships) {
		instructions.push_back(relationship.toInstruction(processors));
		PqlReference lhsRef = relationship.getLhs();
		PqlReference rhsRef = relationship.getRhs();
		PQLEvaluator::insertGetAllInstr(lhsRef, instructions);
		PQLEvaluator::insertGetAllInstr(rhsRef, instructions);
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

void PQLEvaluator::insertGetAllInstr(PqlReference pqlRef, std::vector<Instruction*>& instructions) {
	std::unordered_map<std::string, EntityType> declarations = parsedQuery.getDeclarations();
	if (isSynonymRef(pqlRef) && parsedQuery.isStmtSubtype(pqlRef)) {
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
	EvaluatedTable& evaluatedTable) {
	std::unordered_set<std::string> columnsProjected = parsedQuery.getColumns();
	std::vector<PqlReference> attributesProjected = parsedQuery.getAttributes();
	std::unordered_map<std::string, EntityType> declarations = parsedQuery.getDeclarations();
	std::unordered_map<std::string, std::vector<int>> table = evaluatedTable.getTableRef();
	std::unordered_map<std::string, std::vector<int>> resultTable;
	EvaluatedTable resultEvTable;

	/* If Select-cl is BOOLEAN */
	ProjectionType projType = ParsedQuery::getProjectionType(attributesProjected);
	if (projType == ProjectionType::BOOLEAN) {
		/* Existence of clauses means clauses determine boolean result,
		=> short circuit and go straight to PQLResultProjector */
		if (ParsedQuery::isClausePresent(parsedQuery)) {
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
