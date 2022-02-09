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

    // Assuming correct order of instructions already
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

    switch (instrType) {

    case InstructionType::getAllStmt :
        //PKB's getAllStmts
        std::vector<StmtIndex> results = Entity::getAllStmts();

        // TODO: Generalise this logic below:
        // Convert StmtIndex to string
        // e.g {1, 2, 3}
        //std::vector<VALUE> resultsToStr;
        // std::unordered_map<PqlEntityType, std::vector<VALUE>> newTable = std::unordered_map<PqlEntityType, std::vector<VALUE>>({{PqlEntityType::Stmt, resultsToStr}});

        //for (StmtIndex result : results) {
        //    resultsToStr.push_back((std::to_string(result.getIndex())));
        //}

        // Look into resultsToStr 
       /* currTable = EvaluatedTable(std::unordered_set<PqlEntityType>({ PqlEntityType::Stmt }),
            std::unordered_map<PqlEntityType, std::vector<VALUE>>({ {PqlEntityType::Stmt, resultsToStr} }),
            results.size());*/

        std::vector<VALUE> resultsToStr;
        std::unordered_set<PqlEntityType> PQLtypes;
        PQLtypes.insert(PqlEntityType::Stmt);
        std::unordered_map<PqlEntityType, std::vector<VALUE>> PQLmap;
        for (StmtIndex result : results) {
            resultsToStr.push_back((std::to_string(result.getIndex())));
        }
        PQLmap[PqlEntityType::Stmt] = resultsToStr;
        currTable = EvaluatedTable(PQLtypes, PQLmap, results.size());
        return currTable;
        break;
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
