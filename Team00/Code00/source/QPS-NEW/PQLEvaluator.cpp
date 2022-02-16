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
        PqlRelationshipType pqlRelationshipType =  parsedRelationship.getRelationshipType();
        instructions.push_back(new RelationshipInstruction(pqlRelationshipType, parsedRelationship.getLhs(), parsedRelationship.getRhs()));
    }

    // TODO:
    // 3. Get all pattern results from pattern-clause

    // TODO: Optimisation: Sort instructions.
    return instructions;
}

EvaluatedTable PQLEvaluator::executeInstructions(std::vector<Instruction*> instructions) {
    EvaluatedTable resultEvTable;
    // Test if needed
    //std::unordered_map<std::string, std::vector<int>> testTable;
    //std::unordered_map<std::string, std::vector<int>> testTable1;
    //std::vector<int> vec;
    //vec.push_back(1);
    //vec.push_back(2);
    //vec.push_back(3);
    //std::vector<int> vec2;
    //vec2.push_back(4);
    //vec2.push_back(5);
    //vec2.push_back(6);
    //std::vector<int> vec3;
    //vec3.push_back(2);
    //vec3.push_back(2);
    //vec3.push_back(3);
    //testTable["a"] = vec;
    //testTable["s"] = vec2;
    //testTable1["w"] = vec3;
    //EvaluatedTable test = EvaluatedTable(testTable);
    //EvaluatedTable test1 = EvaluatedTable(testTable1);
    //std::unordered_set<std::string> currentTableColumns;
    //EvaluatedTable mergeTest = innerJoinMerge(test, test1, currentTableColumns);
    //std::cout << "Test tables\n";
    //std::cout << test.getTableString();  // check Table String
    //std::cout << test1.getTableString();  // check Table String
    //std::cout << mergeTest.getTableString();  // check Table String
    std::unordered_set<std::string> currentTableColumns;
    for (size_t i = 0; i < instructions.size(); i++) {
        Instruction* instruction = instructions.at(i);
        EvaluatedTable evTable = instruction->execute();
        resultEvTable = resultEvTable.innerJoinMerge(evTable);    // should merge instead
        std::cout << evTable.getTableString();  // check Table String
    }
    return resultEvTable;
}
