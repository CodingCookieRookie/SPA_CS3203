#include "PQLResultProjector.h"
#include "EvaluatedTable.h"
#include <string>
#include <list>
#include <unordered_set>
#include "../PKB/Entity.h"

std::list<std::string> PQLResultProjector::resolveTableToResults() {
    std::unordered_map<PqlEntityType, std::vector<VALUE>>* table = evaluatedTable.getTableRef();
    std::list<std::string> resList;

    //TODO:
    //for (std::pair mappings : *table) {  // should pass by reference
    //	std::vector<VALUE> values = mappings.second;
    //	std::string res = "";

    //	// Given { {Stmt, {1, 2, 3}} }, Forms "1 2 3"
    //	for (size_t i = 0; i < values.size(); i++) {
    //		res += values.at(i);
    //		if (i != values.size() - 1) {
    //			res += " ";
    //		}
    //	}

    //	// TODO: { {Stmt, {1, 3, 4}, {ASGN, {2, 2, 3}} },
    //	// Forms {"1, 2", "3, 2", "4, 3"} or 1 2 3 2 4 3 according to AutoTester
    //	// A different for loop


    //	resList.push_back(res);
    //}


    for (StmtIndex stmt : Entity::getAllStmts()) {
        resList.push_back(std::to_string(stmt.getIndex()));
    }
    
    return resList;
}

PQLResultProjector::PQLResultProjector(EvaluatedTable evTable) {
    this->evaluatedTable = evTable;
}


PQLResultProjector::PQLResultProjector() {
}