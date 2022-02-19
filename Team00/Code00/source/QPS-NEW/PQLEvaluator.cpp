#include "PQLEvaluator.h"
#include "QPSCommons.h"
#include "Instruction.h"
#include "../PKB/Entity.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

PQLEvaluator::PQLEvaluator(ParsedQuery parsedQuery) :
	parsedQuery(parsedQuery) {};

EvaluatedTable PQLEvaluator::evaluate() {
	std::vector<Instruction*> instructions = PQLEvaluator::evaluateToInstructions(parsedQuery);
	EvaluatedTable resultingEvTable = executeInstructions(instructions);
	return resultingEvTable;
}

std::vector<Instruction*> PQLEvaluator::evaluateToInstructions(ParsedQuery pq) {
	std::vector<Instruction*> instructions = std::vector<Instruction*>();
	std::unordered_map<std::string, PqlEntityType> declarations = pq.getDeclarations();
	std::vector<std::string> columns = pq.getColumns();
	std::vector<ParsedRelationship> relationships = pq.getRelationships();
	std::vector<ParsedPattern> patterns = pq.getPatterns();

	// Assumption: Semantically corrct ParsedQuery
	// 1. Get all entities from Select-clause
	for (size_t i = 0; i < columns.size(); i++) {
		instructions.push_back(new GetAllInstruction(declarations.at(columns[i]), columns[i]));
	}

	// 2. Get all relationship results from such-that-clause
	for (size_t i = 0; i < relationships.size(); i++) {
		ParsedRelationship parsedRelationship = relationships.at(i);
		instructions.push_back(new RelationshipInstruction(parsedRelationship.getRelationshipType(), parsedRelationship.getLhs(), parsedRelationship.getRhs()));
	}

    // 3. Get all pattern results from pattern-clause
    for (size_t i = 0; i < patterns.size(); i++) {
        ParsedPattern parsedPattern = patterns.at(i);
        instructions.push_back(new PatternInstruction(parsedPattern.getSynonym(), parsedPattern.getEntRef(), parsedPattern.getExpression()));
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
