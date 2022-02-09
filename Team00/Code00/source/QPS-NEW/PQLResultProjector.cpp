#include "PQLResultProjector.h"
#include "EvaluatedTable.h"
#include "../PKB/Entity.h"
#include "QPSCommons.h"

#include <string>
#include <list>
#include <unordered_set>

std::list<std::string> PQLResultProjector::resolveTableToResults() {
	std::unordered_map<PqlEntityType, std::vector<VALUE>>* table = evaluatedTable.getTableRef();
	std::list<std::string> resList;
	// for (std::pair mappings : *table) {  // should pass by reference
		// mappings.first -> synonym i.e a
		// mappings.second -> Each number/string value corresponding to synonym i.e {"varName1", "varName2"}
		
		// if Single mapping
	if (table->size() == 1) {
		std::vector<VALUE> values = table->begin()->second;
		std::string res = "";
		// Given { {STMT, {1, 2, 3}} }, Forms "1 2 3"
		for (size_t i = 0; i < values.size(); i++) {
			res += values.at(i);
			if (i != values.size() - 1) {
				res += " ";
			}
			resList.push_back(values.at(i));	// each string in the list will be separated by space in the result
		}
	}
		// else (more than 1 mapping, need to interlace answers 

		// resList.push_back(res);
		// TODO: { {STMT, {1, 3, 4}, {ASGN, {2, 2, 3}} },
		// Forms {"1, 2", "3, 2", "4, 3"} or 1 2 3 2 4 3 according to AutoTester
		// A different for loop
	return resList;
}

PQLResultProjector::PQLResultProjector(EvaluatedTable evTable) {
    this->evaluatedTable = evTable;
}


PQLResultProjector::PQLResultProjector() {
}