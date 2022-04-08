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

void PQLEvaluator::insertGetAllInstr(PqlReference pqlRef, ParsedQuery& pq, std::vector<Instruction*>& instructions) {
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
				Instruction* getAll = getAllSynonym.toInstruction(pkbGetter);
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
			Instruction* getAll = getAllSynonym.toInstruction(pkbGetter);
			EvaluatedTable evTable = getAll->execute();
			resultEvTable = resultEvTable.innerJoinMerge(evTable);
		}
	}
	return resultEvTable;
}
