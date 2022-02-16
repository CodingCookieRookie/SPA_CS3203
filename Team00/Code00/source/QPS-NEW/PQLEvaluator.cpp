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

    // TODO:
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
        resultEvTable = innerJoinMerge(resultEvTable, evTable, currentTableColumns);    // should merge instead
        std::cout << evTable.getTableString();  // check Table String
    }
    return resultEvTable;
}



EvaluatedTable PQLEvaluator::innerJoinMerge(EvaluatedTable currentTable, EvaluatedTable newTable, std::unordered_set<std::string> currentTableColumns) {  // We assume the number of cols for currentTable is non-decreasing
    //EvaluatedTable mergedTable;
    // first check if newTable col has same as that of currentTable
    //std::unordered_map<std::string, std::vector<int>> currentTableHm = currentTable.getTableRef();
    //std::unordered_map<std::string, std::vector<int>> newTableHm = newTable.getTableRef();
    //std::unordered_map<std::string, std::vector<int>> mergedTableHm;
    //int numOfSameSynonymsInNewTable = 0;
    //std::vector<std::string> duplicateSynonyms;
    //for (auto& it : newTableHm) {
    //    if (currentTableColumns.find(it.first) != currentTableColumns.end()) {
    //        numOfSameSynonymsInNewTable++;
    //    }
    //    else {
    //        duplicateSynonyms.push_back(it.first);
    //    }
    //}
    ////std::unordered_map<std::string, std::vector<VALUE>> *table;
    //if (numOfSameSynonymsInNewTable == 0) { // if key and var not present in currentTable do crossproduct
    //// Do cross product
    //    std::cout << "zero\n";
    //    for (size_t i = 0; i < currentTable.getNumRow(); i++) {    //--!-- getNumRow() should keep updating num of rows (num of values in each vector<VALUE>)
    //        for (auto& it : currentTableHm) { // for each col in original
    //            std::string col = it.first;
    //            for (size_t j = 0; j < newTable.getNumRow(); j++) { // for each col in new
    //                mergedTableHm[col].push_back(currentTableHm[col].at(i));
    //            }
    //        }
    //        for (auto& it : newTableHm) {   // For each row in original, and for each new_col in newTableHm, just add the entire vector under each col in newTableHm to mergedTable[new_col]
    //            mergedTableHm[it.first].insert(mergedTableHm[it.first].end(), newTableHm[it.first].begin(), newTableHm[it.first].end());
    //        }
    //    }
    //}
    //else if (numOfSameSynonymsInNewTable == 1) {
    //    std::cout << "one";
    //    // filter out all rows in stmtRef column that are not common
    //    std::string duplicate = duplicateSynonyms.at(0);
    //    std::vector<int> originalTargetColumnValues = currentTableHm[duplicate];
    //    std::vector<int> newTargetColumnValues = newTableHm[duplicate];
    //    for (size_t i = 0; i < originalTargetColumnValues.size(); i++) {   // same as numOfRows
    //        std::vector<int>::iterator indexInNewTargetColumn = std::find(newTargetColumnValues.begin(), newTargetColumnValues.end(), originalTargetColumnValues.at(i));    // find duplicate
    //        if (indexInNewTargetColumn == newTargetColumnValues.end()) {   // check if value of target column in original table exists in new table target column
    //            // if value does not exist/match, ignore row
    //            continue;
    //        }
    //        for (auto& it : currentTableHm) {
    //            std::string col = it.first;
    //            mergedTableHm[col].push_back(currentTableHm[col].at(i));
    //        }
    //        for (auto& it : newTableHm) {
    //            std::string col = it.first;
    //            if (col == duplicate) {
    //                continue;   // we don't have to push the same target col of new table
    //            }
    //            mergedTableHm[col].push_back(newTableHm[col].at(indexInNewTargetColumn - newTargetColumnValues.begin()));
    //        }
    //    }
    //} else {  // both stmtRef and entRef (or more) exists in original table 
    //    std::cout << "two";
    //    std::vector<int> filteredIndexesForOriginalTargetColumnValuesEnt;                   // all indices stored here are w.r.t originalTargetColumnValuesEnt
    //    std::unordered_map<int, int> filteredIndexesForNewTargetColumnValuesEnt;    // all indices stored here are w.r.t newTargetColumnValuesEnt, key = index of row in original table , val = index of row in new table

    //    //--!-- Warning this will only work if there is no exact duplicate entry in either table eg newTable = {{1,2}, {1,2}}
    //    std::string duplicate = duplicateSynonyms.at(0);
    //    std::vector<int> originalTargetColumnValuesStmt = currentTableHm[duplicate];
    //    std::vector<int> newTargetColumnValuesStmt = newTableHm[duplicate];

    //    // use rows of first duplicate entity as reference
    //    for (size_t i = 0; i < originalTargetColumnValuesStmt.size(); i++) {   // same as numOfRows
    //        std::vector<int>::iterator indexInNewTargetColumn = std::find(newTargetColumnValuesStmt.begin(), newTargetColumnValuesStmt.end(), originalTargetColumnValuesStmt.at(i));
    //        if (indexInNewTargetColumn == newTargetColumnValuesStmt.end()) {   // check if value of target column in original table exists in new table target column
    //            // if value does not exist/match, ignore row
    //            continue;
    //        }
    //        filteredIndexesForOriginalTargetColumnValuesEnt.push_back(i);
    //        filteredIndexesForNewTargetColumnValuesEnt.insert({ originalTargetColumnValuesStmt.at(i), indexInNewTargetColumn - newTargetColumnValuesStmt.begin() });
    //    }

    //    // for subsequent duplicate entities, check if it contains the first duplicate entity row, if no => filter out
    //    for (size_t j = 1; j < duplicateSynonyms.size(); j++) {
    //        duplicate = duplicateSynonyms.at(j);
    //        originalTargetColumnValuesStmt = currentTableHm[duplicate]; 
    //        newTargetColumnValuesStmt = newTableHm[duplicate];
    //        for (size_t i = 0; i < originalTargetColumnValuesStmt.size(); i++) {   // same as numOfRows
    //            std::vector<int>::iterator indexInNewTargetColumn = std::find(newTargetColumnValuesStmt.begin(), newTargetColumnValuesStmt.end(), originalTargetColumnValuesStmt.at(i));
    //            if (indexInNewTargetColumn == newTargetColumnValuesStmt.end()) {   // check if value of target column in original table exists in new table target column
    //                // if value does not exist/match, ignore row
    //                continue;
    //            }
    //            // update filteredList
    //            std::vector<int>::iterator sameIndex = std::find(filteredIndexesForOriginalTargetColumnValuesEnt.begin(), filteredIndexesForOriginalTargetColumnValuesEnt.end(), originalTargetColumnValuesStmt.at(i));
    //            if (sameIndex == filteredIndexesForOriginalTargetColumnValuesEnt.end()) {
    //                filteredIndexesForOriginalTargetColumnValuesEnt.erase(std::remove(filteredIndexesForOriginalTargetColumnValuesEnt.begin(),
    //                    filteredIndexesForOriginalTargetColumnValuesEnt.end(), i), filteredIndexesForOriginalTargetColumnValuesEnt.end());; // remove not same index by index value
    //                filteredIndexesForNewTargetColumnValuesEnt.erase(i);    
    //            }
    //        }
    //        
    //    }
    //    // add all filtered rows into new mergedTable
    //    for (size_t i = 0; i < filteredIndexesForOriginalTargetColumnValuesEnt.size(); i++) {
    //        int originalFilteredIndex = filteredIndexesForOriginalTargetColumnValuesEnt.at(i);
    //        for (auto& it : currentTableHm) {  // since both columns exist in original table, there is no need to push for the same target columns from new table
    //            std::string col = it.first;
    //            mergedTableHm[col].push_back(currentTableHm[col].at(originalFilteredIndex));
    //        }
    //    }
    //}
    //return EvaluatedTable(mergedTableHm);
    return EvaluatedTable();
}
