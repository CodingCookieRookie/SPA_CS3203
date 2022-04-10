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

std::vector<Instruction*> PQLEvaluator::getBooleanClauses(const std::vector<Instruction*>& instructions) {
	/* Filter out boolean clauses - clauses that don't involve any synonyms */
	std::vector<Instruction*> booleanClauses;
	for (Instruction* instruction : instructions) {
		std::unordered_set<std::string> synonyms = instruction->getSynonyms();
		if (synonyms.empty()) {
			booleanClauses.push_back(instruction);
		}
	}
	return booleanClauses;
}

std::unordered_map<std::string, std::vector<Instruction*>> PQLEvaluator::buildInstructionGraph(const std::vector<Instruction*>& instructions) {
	/* Build "Adjacency List" of instructions */
	std::unordered_map<std::string, std::vector<Instruction*>> adjList;
	for (Instruction* instruction : instructions) {
		std::unordered_set<std::string> synonyms = instruction->getSynonyms();
		for (const std::string& synonym : synonyms) {
			adjList[synonym].push_back(instruction);
		}
	}
	return adjList;
}

std::vector<std::pair<int, std::string>> PQLEvaluator::getConnectedComponents(
	const std::unordered_map<std::string, std::vector<Instruction*>>& adjList) {
	/* Represent each connected component by a <score, synonym> tuple,
	where synonym is any synonym in that connected component */
	std::vector<std::pair<int, std::string>> connectedComponents;
	size_t numSynonyms = adjList.size();
	std::unordered_set<std::string> visitedSynonyms;
	std::unordered_set<Instruction*> visitedInstructions;
	visitedSynonyms.reserve(numSynonyms);
	for (const auto& [synonym, clauses] : adjList) {
		if (visitedSynonyms.find(synonym) != visitedSynonyms.end()) {
			continue;
		}
		int sumDifficulty = 0;
		std::queue<std::string> visitNext;
		visitNext.push(synonym);
		visitedSynonyms.insert(synonym);
		while (!visitNext.empty()) {
			std::string currSynonym = visitNext.front();
			visitNext.pop();
			for (Instruction* instruction : adjList.at(currSynonym)) {
				std::unordered_set<std::string> synonyms = instruction->getSynonyms();
				for (const std::string& nextSynonym : synonyms) {
					if (visitedInstructions.find(instruction) == visitedInstructions.end()) {
						sumDifficulty += instruction->getDifficultyScore();
						visitedInstructions.insert(instruction);
					}
					if (visitedSynonyms.find(nextSynonym) == visitedSynonyms.end()) {
						visitNext.push(nextSynonym);
						visitedSynonyms.insert(nextSynonym);
					}
				}
			}
		}
		connectedComponents.emplace_back(sumDifficulty, synonym);
	}
	std::sort(connectedComponents.begin(), connectedComponents.end(),
		[](const auto& p1, const auto& p2) { return p1.first < p2.first; });
	return connectedComponents;
}

EvaluatedTable PQLEvaluator::executeBooleanClauses(const std::vector<Instruction*>& instructions) {
	EvaluatedTable resultEvTable;
	for (Instruction* instruction : instructions) {
		if (!resultEvTable.getEvResult()) {
			break;
		}
		EvaluatedTable evTable = instruction->execute();
		resultEvTable = resultEvTable.innerJoinMerge(evTable);
	}
	return resultEvTable;
}

EvaluatedTable PQLEvaluator::executeInstructionGraph(
	const std::unordered_map<std::string, std::vector<Instruction*>>& adjList,
	const std::vector<std::pair<int, std::string>>& connectedComponents) {
	EvaluatedTable resultEvTable;
	for (const auto& [score, synonym] : connectedComponents) {
		EvaluatedTable projectedEvTable = executeConnectedComponent(adjList, synonym);
		resultEvTable = resultEvTable.innerJoinMerge(projectedEvTable);
	}
	return resultEvTable;
}

EvaluatedTable PQLEvaluator::executeConnectedComponent(
	const std::unordered_map<std::string, std::vector<Instruction*>>& adjList,
	const std::string& synonym) {
	std::set<std::pair<int, Instruction*>> visitNextInstructions;
	std::unordered_set<Instruction*> visitedInstructions;
	for (Instruction* instruction : adjList.at(synonym)) {
		visitedInstructions.insert(instruction);
		visitNextInstructions.emplace(instruction->getDifficultyScore(), instruction);
	}
	EvaluatedTable groupEvTable;
	while (!visitNextInstructions.empty()) {
		auto [score, instruction] = *visitNextInstructions.begin();
		visitNextInstructions.erase(visitNextInstructions.begin());
		/* Add all the clauses which are "connected" to the current clause
		to the frontier (if they haven't already been added/executed). */
		std::unordered_set<std::string> synonyms = instruction->getSynonyms();
		for (const std::string& nextSynonym : synonyms) {
			for (Instruction* nextInstruction : adjList.at(nextSynonym)) {
				if (visitedInstructions.find(nextInstruction) != visitedInstructions.end()) {
					continue;
				}
				visitedInstructions.insert(nextInstruction);
				visitNextInstructions.emplace(nextInstruction->getDifficultyScore(), nextInstruction);
			}
		}
		EvaluatedTable evTable = instruction->execute();
		groupEvTable = groupEvTable.innerJoinMerge(evTable);
	}
	EvaluatedTable projectedEvTable = groupEvTable.project(parsedQuery.getColumns());
	return projectedEvTable;
}

EvaluatedTable PQLEvaluator::executeInstructions(const std::vector<Instruction*>& instructions) {
	EvaluatedTable resultEvTable;

	std::unordered_map<std::string, std::vector<Instruction*>> adjList = buildInstructionGraph(instructions);
	std::vector<Instruction*> booleanClauses = getBooleanClauses(instructions);

	std::vector<std::pair<int, std::string>> connectedComponents = getConnectedComponents(adjList);

	EvaluatedTable boolClauseEvTable = executeBooleanClauses(booleanClauses);
	if (!boolClauseEvTable.getEvResult()) {
		return boolClauseEvTable;
	}
	EvaluatedTable graphClauseEvTable = executeInstructionGraph(adjList, connectedComponents);
	return graphClauseEvTable;
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
		/* No Clauses, implicit true */
		return EvaluatedTable(true);
	}
}

EvaluatedTable PQLEvaluator::handleNonBoolean(EvaluatedTable& evaluatedTable) {
	/* For each column that already exists in the final EvTable, take it from the evaluatedTable */
	Table resultTable = PQLEvaluator::populateTable(evaluatedTable);
	/* If the evaluated table is false or an empty table, use a false table */
	EvaluatedTable resultEvTable = EvaluatedTable(resultTable);
	Table table = evaluatedTable.getTableRef();
	if (evaluatedTable.getEvResult() == false || (table.size() > 0 && evaluatedTable.getNumRow() == 0)) {
		resultEvTable = EvaluatedTable(false);
	}
	/* For each column that does not exist in the final EvTable,
	get it via an instruction and then perform a cross product. */
	resultEvTable = PQLEvaluator::fillInColumns(resultEvTable, evaluatedTable);
	return resultEvTable;
}

Table PQLEvaluator::populateTable(EvaluatedTable& evaluatedTable) {
	Table resultTable;
	std::unordered_set<std::string> columnsProjected = parsedQuery.getColumns();
	Table table = evaluatedTable.getTableRef();
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
	Table table = evaluatedTable.getTableRef();
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
