#include "PQLEvaluator.h"

Processors PQLEvaluator::instantiateProcessors() {
	NextTProcessor* nextTProcessor = new NextTProcessor(new NextTCache());
	AffectsProcessor* affectsProcessor = new AffectsProcessor();
	AffectsTProcessor* affectsTProcessor = new AffectsTProcessor(new AffectsTCache(), affectsProcessor);
	Processors processors = Processors(nextTProcessor, affectsProcessor, affectsTProcessor);
	return processors;
}

PQLEvaluator::PQLEvaluator(ParsedQuery& parsedQuery, PKBGetter* pkbGetter) :
	parsedQuery(parsedQuery), pkbGetter(pkbGetter), processors(PQLEvaluator::instantiateProcessors()) {}

EvaluatedTable PQLEvaluator::evaluate() {
	std::vector<Instruction*> instructions;
	try {
		instructions = parsedQuery.getClauseInstructions(processors, pkbGetter);
	} catch (QPSException&) {
		return EvaluatedTable(false);
	}
	EvaluatedTable resultingEvTable = executeInstructions(instructions);
	return resultingEvTable;
}

EvaluatedTable PQLEvaluator::selectProjection(EvaluatedTable& resultingEvTable) {
	EvaluatedTable projectingEvTable = PQLEvaluator::selectColumnsForProjection(resultingEvTable, parsedQuery);
	return projectingEvTable;
}

std::vector<Instruction*> PQLEvaluator::evaluateToInstructions(ParsedQuery pq) {
	std::vector<Instruction*> instructions = std::vector<Instruction*>();
	return instructions;
}

void PQLEvaluator::insertGetAllInstr(PqlReference pqlRef, ParsedQuery& pq, std::vector<Instruction*>& instructions) {}

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
	std::vector<PqlReference> attributesProjected = parsedQuery.getAttributes();
	ProjectionType projType = ParsedQuery::getProjectionType(attributesProjected);
	if (projType == ProjectionType::BOOLEAN) {
		return PQLEvaluator::handleBoolean(evaluatedTable);
	}
	return PQLEvaluator::handleNonBoolean(evaluatedTable);
}

EvaluatedTable PQLEvaluator::handleBoolean(EvaluatedTable& evaluatedTable) {
	/* Existence of clauses means clauses determine boolean result,
	=> short circuit and go straight to PQLResultProjector */
	if (ParsedQuery::isClausePresent(parsedQuery)) {
		return evaluatedTable;
	} else {
		/* No Clauses, existence of declared synonyms determine boolean result
		=> populate declarations into table */
		EvaluatedTable resultEvTable;
		return PQLEvaluator::populateDeclarations(resultEvTable);
	}
}

EvaluatedTable& PQLEvaluator::populateDeclarations(EvaluatedTable& resultEvTable) {
	std::unordered_map<std::string, EntityType> declarations = parsedQuery.getDeclarations();
	for (const std::pair<std::string, EntityType>& synonym : declarations) {
		//EntityType columnType = declarations.at(column);
		ParsedGetAll getAllSynonym = ParsedGetAll(synonym.second, synonym.first);
		Instruction* getAll = getAllSynonym.toInstruction(pkbGetter);
		EvaluatedTable evTable = getAll->execute();
		resultEvTable = resultEvTable.innerJoinMerge(evTable);
	}
	return resultEvTable;
}

EvaluatedTable PQLEvaluator::handleNonBoolean(EvaluatedTable& evaluatedTable) {
	/* For each column that already exists in the final EvTable, take it from the evaluatedTable */
	std::unordered_map<std::string, std::vector<int>> resultTable = PQLEvaluator::populateTable(evaluatedTable);
	/* If the evaluated table is false or an empty table, use a false table */
	EvaluatedTable resultEvTable = EvaluatedTable(resultTable);
	std::unordered_map<std::string, std::vector<int>> table = evaluatedTable.getTableRef();
	if (evaluatedTable.getEvResult() == false || (table.size() > 0 && evaluatedTable.getNumRow() == 0)) {
		resultEvTable = EvaluatedTable(false);
	}
	/* For each column that does not exist in the final EvTable,
	get it via an instruction and then perform a cross product. */
	resultEvTable = PQLEvaluator::fillInColumns(resultEvTable, evaluatedTable);
	return resultEvTable;
}

std::unordered_map<std::string, std::vector<int>> PQLEvaluator::populateTable(EvaluatedTable& evaluatedTable) {
	std::unordered_map<std::string, std::vector<int>> resultTable;
	std::unordered_set<std::string> columnsProjected = parsedQuery.getColumns();
	std::unordered_map<std::string, std::vector<int>> table = evaluatedTable.getTableRef();
	for (const std::string& column : columnsProjected) {
		if (table.find(column) != table.end()) {
			resultTable[column] = table[column];
		}
	}
	return resultTable;
}

EvaluatedTable& PQLEvaluator::fillInColumns(EvaluatedTable& resultEvTable, EvaluatedTable& evaluatedTable) {
	std::unordered_set<std::string> columnsProjected = parsedQuery.getColumns();
	std::unordered_map<std::string, EntityType> declarations = parsedQuery.getDeclarations();
	std::unordered_map<std::string, std::vector<int>> table = evaluatedTable.getTableRef();
	for (const std::string& column : columnsProjected) {
		if (table.find(column) == table.end()) {
			EntityType columnType = declarations.at(column);
			ParsedGetAll getAllSynonym = ParsedGetAll(columnType, column);
			Instruction* getAll = getAllSynonym.toInstruction(pkbGetter);
			EvaluatedTable evTable = getAll->execute();
			resultEvTable = resultEvTable.innerJoinMerge(evTable);
		}
	}
	return resultEvTable;
}
