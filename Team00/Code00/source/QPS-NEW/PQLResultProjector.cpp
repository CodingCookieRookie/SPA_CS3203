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

	// E.g.
	// { {STMT, {1, 3, 4}, {ASGN, {2, 2, 3}} },
	// Forms {"1, 2", "3, 2", "4, 3"} or 1 2 3 2 4 3 according to AutoTester
	int numRow = evaluatedTable.getNumRow();
	for (int i = 0; i < numRow; i++) {	// for each row
		std::unordered_map<PqlEntityType, std::vector<VALUE>>::iterator it = table->begin();
		std::string res = "";
		while (it != table->end()) {	// for each col
			res += it->second[i];
			resList.push_back(res);
			it++;
		}
	}
	return resList;
}

PQLResultProjector::PQLResultProjector(EvaluatedTable evTable) {
    this->evaluatedTable = evTable;
}


PQLResultProjector::PQLResultProjector() {
}