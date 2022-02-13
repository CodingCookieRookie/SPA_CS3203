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
    // 1. Check if entitiy in Select clause is found in declarations
    for (std::string entity : columns) {
        PqlEntityType entityTypeRequired = declarations.at(entity);
        switch (entityTypeRequired) {
        case PqlEntityType::Stmt :
            instructions.push_back(&GetAllInstruction(GetAllInstructionType::getAllStmt));
            break;
        case PqlEntityType::Print :
            instructions.push_back(&GetAllInstruction(GetAllInstructionType::getAllPrint));
            break;
        case PqlEntityType::Call :
            instructions.push_back(&GetAllInstruction(GetAllInstructionType::getAllCall));
            break;
        case PqlEntityType::While :
            instructions.push_back(&GetAllInstruction(GetAllInstructionType::getAllWhile));
            break;
        case PqlEntityType::If :
            instructions.push_back(&GetAllInstruction(GetAllInstructionType::getAllIf));
            break;
        case PqlEntityType::Assign :
            instructions.push_back(&GetAllInstruction(GetAllInstructionType::getAllAsgn));
            break;
        case PqlEntityType::Variable :
            instructions.push_back(&GetAllInstruction(GetAllInstructionType::getAllVar));
            break;
        case PqlEntityType::Constant:
            instructions.push_back(&GetAllInstruction(GetAllInstructionType::getAllConst));
            break;
        case PqlEntityType::Procedure:
            instructions.push_back(&GetAllInstruction(GetAllInstructionType::getAllProc));
            break;
        }
    }

    //std::vector<ParsedRelationship> relationships = pq.getRelationships();
    for (size_t i = 0; i < relationships.size(); i++) {
        ParsedRelationship parsedRelationship = relationships.at(i);
        PqlRelationshipType pqlRelationshipType =  parsedRelationship.getRelationshipType();
        instructions.push_back(&RelationshipInstruction(pqlRelationshipType, parsedRelationship.getLhs().second, parsedRelationship.getRhs().second));
    }

    // TODO:
    // 2. If there are relations
    //		checks for what relations are included in the ParsedQuery
    //		Add respective instructions
    // 3. If there are patterns
    //		checks for what patterns are included in the ParsedQuery
    //		Add respective instructions

    // TODO: Optimisation: Sort instructions.
    return instructions;
}

EvaluatedTable PQLEvaluator::executeInstructions(std::vector<Instruction*> instructions) {
    EvaluatedTable resultEvTable = EvaluatedTable();

    //// Assumption: Correct order of instructions
    //// Call relevant API
    //for (Instruction instruction : instructions) {
    //    EvaluatedTable currEvTable = execute(instruction);
    //    if (resultEvTable.getEntities().empty()) {
    //        resultEvTable = currEvTable;
    //    }
    //    else {
    //        // TODO: Merge Tables (for Select and Pattern clause in the future)
    //        // resultEvTable = innerJoinMerge(resultEvTable, currEvTable);
    //    }
    //}

    return resultEvTable;
}

//EvaluatedTable PQLEvaluator::execute(Instruction instr) {
//    //Instruction instrType = instr.getClassType();
//
//    EvaluatedTable currTable;
//    std::unordered_set<PqlEntityType> PQLtypes;
//    std::unordered_map<PqlEntityType, std::vector<VALUE>> PQLmap;
//
//    //switch (instrType) {
//        //case GetAllInstructionType::getAllStmt: {
//        //    std::vector<StmtIndex> results = Entity::getAllStmts();
//        //    std::vector<VALUE> resultsToStr;
//        //    PQLtypes.insert(PqlEntityType::Stmt);
//        //    // Convert StmtIndex to string, e.g {1, 2, 3}  
//        //    for (StmtIndex result : results) {
//        //        resultsToStr.push_back((std::to_string(result.getIndex())));
//        //    }
//        //    PQLmap[PqlEntityType::Stmt] = resultsToStr;
//        //    currTable = EvaluatedTable(PQLtypes, PQLmap, results.size());
//        //    break;
//        //}
//        //case GetAllInstructionType::getAllPrint :
//            //    std::vector<StmtIndex> results = Entity::getAllPrint();
//            //    std::vector<VALUE> resultsToStr;
//            //    PQLtypes.insert(PqlEntityType::Print);
//            //    // Convert StmtIndex to string, e.g {1, 2, 3}  
//            //    for (StmtIndex result : results) {
//            //        resultsToStr.push_back((std::to_string(result.getIndex())));
//            //    }
//            //    PQLmap[PqlEntityType::Print] = resultsToStr;
//            //    currTable = EvaluatedTable(PQLtypes, PQLmap, results.size());
//            //    break;
//        /*case GetAllInstructionType::getAllVar: {
//            std::vector<std::string> results = Entity::getAllVars();
//            PQLtypes.insert(PqlEntityType::Variable);
//            PQLmap[PqlEntityType::Variable] = results;
//            currTable = EvaluatedTable(PQLtypes, PQLmap, results.size());
//            break;
//        }
//        case GetAllInstructionType::getAllProc: {
//            std::vector<std::string> results = Entity::getAllProcs();
//            PQLtypes.insert(PqlEntityType::Procedure);
//            PQLmap[PqlEntityType::Procedure] = results;
//            currTable = EvaluatedTable(PQLtypes, PQLmap, results.size());
//            break;
//        }
//        case GetAllInstructionType::getAllConst: {
//            std::vector<int> results = Entity::getAllConsts();
//            std::vector<VALUE> resultsToStr;
//            for (int result : results) {
//                resultsToStr.emplace_back((std::to_string(result)));
//            }
//            PQLtypes.insert(PqlEntityType::Constant);
//            PQLmap[PqlEntityType::Constant] = resultsToStr;
//            currTable = EvaluatedTable(PQLtypes, PQLmap, results.size());
//            break;
//        }   */
//   // }
//    return currTable;
//}
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
                //int newTableHmEntryIndex = 0;
                for (auto& it : newTableHm) { // for each col in new
                    mergedTableHm[col].push_back(currentTableHm[col].at(i));
                    //mergedTableHm[it.first].push_back(newTableHm[it.first].at(newTableHmEntryIndex));
                }
                //newTableHmEntryIndex++;
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
