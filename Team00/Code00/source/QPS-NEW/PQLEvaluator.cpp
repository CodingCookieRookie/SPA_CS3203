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
    for (size_t i = 0; i < instructions.size(); i++) {
        Instruction* instruction = instructions.at(i);
        EvaluatedTable evTable = instruction->execute();
        resultEvTable = evTable;    // should merge instead
        std::cout << evTable.getTableString();  // check Table String
    }
    return resultEvTable;
}



// Select x such that pattern a1("x", _ ) => key = a1, entRef = ? or no entRef
// new Table has at most two columns => Modifies(s, v1) / Uses(a, v2) / pattern a("x", _ ) / Modifies(s1, "x") => In all of this we know that there is definitely a stmt index (key/col)
EvaluatedTable PQLEvaluator::innerJoinMerge(EvaluatedTable currentTable, EvaluatedTable newTable, std::string stmtRef, std::string entRef, std::unordered_set<std::string> currentTableColumns) {  // We assume the number of cols for currentTable is non-decreasing
    EvaluatedTable mergedTable;
    // first check if newTable col has same as that of currentTable
    std::unordered_map<std::string, std::vector<int>> currentTableHm = currentTable.getTableRef();
    std::unordered_map<std::string, std::vector<int>> newTableHm = newTable.getTableRef();
    std::unordered_map<std::string, std::vector<int>> mergedTableHm = mergedTable.getTableRef();
    //std::unordered_map<std::string, std::vector<VALUE>> *table;
    if (currentTableColumns.find(stmtRef) == currentTableColumns.end() && currentTableColumns.find(entRef) == currentTableColumns.end()) { // if key and var not present in currentTable do crossproduct
    // Do cross product
        for (int i = 0; i < currentTable.getNumRow(); i++) {    //--!-- getNumRow() should keep updating num of rows (num of values in each vector<VALUE>)
            for (auto& it : currentTableHm) { // for each col in original
                std::string col = it.first; 
                for (auto& it : newTableHm) { // for each col in new
                    mergedTableHm[col].push_back(currentTableHm[col].at(i));
                }
            }
            for (auto& it : newTableHm) {   // For each row in original, and for each new_col in newTableHm, just add the entire vector under each col in newTableHm to mergedTable[new_col]
                mergedTableHm[it.first].insert(mergedTableHm[it.first].end(), newTableHm[it.first].begin(), newTableHm[it.first].end());
            }
        }
    }
    else if (currentTableColumns.find(stmtRef) != currentTableColumns.end()) {
        // filter out all rows in stmtRef column that are not common
        std::vector<int> originalTargetColumnValues = currentTableHm[stmtRef];
        std::vector<int> newTargetColumnValues = newTableHm[stmtRef];
        for (size_t i = 0; i < originalTargetColumnValues.size(); i++) {   // same as numOfRows
            std::vector<int>::iterator indexInNewTargetColumn = std::find(newTargetColumnValues.begin(), newTargetColumnValues.end(), originalTargetColumnValues.at(i));
            if (indexInNewTargetColumn == newTargetColumnValues.end()) {   // check if value of target column in original table exists in new table target column
                // if value does not exist/match, ignore row
                continue;
            }
            for (auto& it : currentTableHm) {
                std::string col = it.first;
                mergedTableHm[col].push_back(currentTableHm[col].at(i));  
            }
            for (auto& it : newTableHm) {
                std::string col = it.first;
                if (col == stmtRef) {
                    continue;   // we don't have to push the same target col of new table
                }
                mergedTableHm[col].push_back(newTableHm[col].at(indexInNewTargetColumn - newTargetColumnValues.begin()));
            }
        }
    }
    else if (currentTableColumns.find(entRef) != currentTableColumns.end()) {   // Not sure if this is needed? Personally I think it's required
        // filter out all rows in entRef column that are not common
        std::vector<int> originalTargetColumnValues = currentTableHm[entRef];
        std::vector<int> newTargetColumnValues = newTableHm[entRef];
        for (size_t i = 0; i < originalTargetColumnValues.size(); i++) {   // same as numOfRows
            std::vector<int>::iterator indexInNewTargetColumn = std::find(newTargetColumnValues.begin(), newTargetColumnValues.end(), originalTargetColumnValues.at(i));
            if (indexInNewTargetColumn == newTargetColumnValues.end()) {   // check if value of target column in original table exists in new table target column
                // if value does not exist/match, ignore row
                continue;
            }
            for (auto& it : currentTableHm) {
                std::string col = it.first;
                mergedTableHm[col].push_back(currentTableHm[col].at(i));  
            }
            for (auto& it : newTableHm) {
                std::string col = it.first;
                if (col == entRef) {
                    continue;   // we don't have to push the same target col of new table
                }
                mergedTableHm[col].push_back(newTableHm[col].at(indexInNewTargetColumn - newTargetColumnValues.begin()));
            }
        }
    }
    else {  // both stmtRef and entRef exists in original table 
        std::vector<int> originalTargetColumnValuesStmt = currentTableHm[stmtRef];
        std::vector<int> newTargetColumnValuesStmt = newTableHm[stmtRef];
        std::vector<int> originalTargetColumnValuesEnt = currentTableHm[entRef];
        std::vector<int> newTargetColumnValuesEnt = newTableHm[entRef];
        std::vector<int> filteredIndexesForOriginalTargetColumnValuesEnt;                   // all indices stored here are w.r.t originalTargetColumnValuesEnt
        std::unordered_map<int, int> filteredIndexesForNewTargetColumnValuesEnt;    // all indices stored here are w.r.t newTargetColumnValuesEnt

        //--!-- Warning this will only work if there is no exact duplicate entry in either table eg newTable = {{1,2}, {1,2}}

        // filter by stmtRef first
        for (size_t i = 0; i < originalTargetColumnValuesStmt.size(); i++) {   // same as numOfRows
            std::vector<int>::iterator indexInNewTargetColumn = std::find(newTargetColumnValuesStmt.begin(), newTargetColumnValuesStmt.end(), originalTargetColumnValuesStmt.at(i));
            if (indexInNewTargetColumn == newTargetColumnValuesStmt.end()) {   // check if value of target column in original table exists in new table target column
                // if value does not exist/match, ignore row
                continue;
            }
            filteredIndexesForOriginalTargetColumnValuesEnt.push_back(i);
            filteredIndexesForNewTargetColumnValuesEnt.insert({ originalTargetColumnValuesStmt.at(i), indexInNewTargetColumn - newTargetColumnValuesEnt.begin() });
        }

        // filter by entRef next
        for (size_t i = 0; i < filteredIndexesForOriginalTargetColumnValuesEnt.size(); i++) {
            int originalFilteredIndex = filteredIndexesForOriginalTargetColumnValuesEnt.at(i);
            int originalEntValue = originalTargetColumnValuesEnt.at(i);
            if (filteredIndexesForNewTargetColumnValuesEnt.find(originalEntValue) != filteredIndexesForNewTargetColumnValuesEnt.end()) {
                int newFilteredIndex = filteredIndexesForNewTargetColumnValuesEnt[originalEntValue];
                int newEntValue = originalTargetColumnValuesEnt.at(newFilteredIndex);
                // check if both ent values with same 
                if (originalEntValue == newEntValue) {
                    for (auto& it : currentTableHm) {  // since both columns exist in original table, there is no need to push for the same target columns from new table
                        std::string col = it.first;
                        mergedTableHm[col].push_back(currentTableHm[col].at(originalFilteredIndex));
                    }
                }
            }
        }
    }
    return mergedTable;
}
