#include "PQLEvaluator.h"
#include "QPSCommons.h"
#include "Instruction.h"
#include "../PKB/Entity.h"

#include <string>
#include <unordered_map>
#include <unordered_set>


PQLEvaluator::PQLEvaluator(ParsedQuery parsedQuery) :
    parsedQuery(parsedQuery) {};

EvaluatedTable PQLEvaluator::evaluate() {
    std::vector<Instruction> instructions = PQLEvaluator::evaluateToInstructions(parsedQuery);
    EvaluatedTable resultingEvTable = executeInstructions(instructions);
    return resultingEvTable;
}


std::vector<Instruction> PQLEvaluator::evaluateToInstructions(ParsedQuery& pq) {
    std::vector<Instruction> instructions = std::vector<Instruction>();
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
            instructions.push_back(Instruction(InstructionType::getAllStmt));
            break;
        case PqlEntityType::Print :
            instructions.push_back(Instruction(InstructionType::getAllPrint));
            break;
        case PqlEntityType::Call :
            instructions.push_back(Instruction(InstructionType::getAllCall));
            break;
        case PqlEntityType::While :
            instructions.push_back(Instruction(InstructionType::getAllWhile));
            break;
        case PqlEntityType::If :
            instructions.push_back(Instruction(InstructionType::getAllIf));
            break;
        case PqlEntityType::Assign :
            instructions.push_back(Instruction(InstructionType::getAllAsgn));
            break;
        case PqlEntityType::Variable :
            instructions.push_back(Instruction(InstructionType::getAllVar));
            break;
        case PqlEntityType::Constant:
            instructions.push_back(Instruction(InstructionType::getAllConst));
            break;
        case PqlEntityType::Procedure:
            instructions.push_back(Instruction(InstructionType::getAllProc));
            break;
        }
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

EvaluatedTable PQLEvaluator::executeInstructions(std::vector<Instruction> instructions) {
    EvaluatedTable resultEvTable = EvaluatedTable();

    // Assumption: Correct order of instructions
    // Call relevant API
    for (Instruction instruction : instructions) {
        EvaluatedTable currEvTable = execute(instruction);
        if (resultEvTable.getEntities().empty()) {
            resultEvTable = currEvTable;
        }
        else {
            // TODO: Merge Tables (for Select and Pattern clause in the future)
            // resultEvTable = innerJoinMerge(resultEvTable, currEvTable);
        }
    }

    return resultEvTable;
}

EvaluatedTable PQLEvaluator::execute(Instruction& instr) {
    InstructionType instrType = instr.getType();
    EvaluatedTable currTable;
    std::unordered_set<PqlEntityType> PQLtypes;
    std::unordered_map<PqlEntityType, std::vector<VALUE>> PQLmap;

    switch (instrType) {
        case InstructionType::getAllStmt: {
            std::vector<StmtIndex> results = Entity::getAllStmts();
            std::vector<VALUE> resultsToStr;
            PQLtypes.insert(PqlEntityType::Stmt);
            // Convert StmtIndex to string, e.g {1, 2, 3}  
            for (StmtIndex result : results) {
                resultsToStr.push_back((std::to_string(result.getIndex())));
            }
            PQLmap[PqlEntityType::Stmt] = resultsToStr;
            currTable = EvaluatedTable(PQLtypes, PQLmap, results.size());
            break;
        }
        //case InstructionType::getAllPrint :
            //    std::vector<StmtIndex> results = Entity::getAllPrint();
            //    std::vector<VALUE> resultsToStr;
            //    PQLtypes.insert(PqlEntityType::Print);
            //    // Convert StmtIndex to string, e.g {1, 2, 3}  
            //    for (StmtIndex result : results) {
            //        resultsToStr.push_back((std::to_string(result.getIndex())));
            //    }
            //    PQLmap[PqlEntityType::Print] = resultsToStr;
            //    currTable = EvaluatedTable(PQLtypes, PQLmap, results.size());
            //    break;
        case InstructionType::getAllVar: {
            std::vector<std::string> results = Entity::getAllVars();
            PQLtypes.insert(PqlEntityType::Variable);
            PQLmap[PqlEntityType::Variable] = results;
            currTable = EvaluatedTable(PQLtypes, PQLmap, results.size());
            break;
        }
        case InstructionType::getAllProc: {
            std::vector<std::string> results = Entity::getAllProcs();
            PQLtypes.insert(PqlEntityType::Procedure);
            PQLmap[PqlEntityType::Procedure] = results;
            currTable = EvaluatedTable(PQLtypes, PQLmap, results.size());
            break;
        }
        case InstructionType::getAllConst: {
            std::vector<int> results = Entity::getAllConsts();
            std::vector<VALUE> resultsToStr;
            for (int result : results) {
                resultsToStr.emplace_back((std::to_string(result)));
            }
            PQLtypes.insert(PqlEntityType::Constant);
            PQLmap[PqlEntityType::Constant] = resultsToStr;
            currTable = EvaluatedTable(PQLtypes, PQLmap, results.size());
            break;
        }   

    }
    return currTable;
}

EvaluatedTable PQLEvaluator::innerJoinMerge(EvaluatedTable& evTable, EvaluatedTable& newEvTable) {
    EvaluatedTable mergedTable;
    // TODO:
    // 1. Short-circuit if other table is empty
    // 2.

    mergedTable = evTable;
    return mergedTable;
}
