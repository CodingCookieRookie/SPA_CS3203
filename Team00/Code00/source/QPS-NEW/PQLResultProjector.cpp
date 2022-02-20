#include "PQLResultProjector.h"
#include "EvaluatedTable.h"
#include "../PKB/Entity.h"
#include "QPSCommons.h"

#include <string>
#include <list>
#include <unordered_set>

std::list<std::string> PQLResultProjector::resolveTableToResults() {
	std::unordered_map<std::string, PqlEntityType> entities = evaluatedTable.getEntities();
	std::unordered_map<std::string, std::vector<int>> table = evaluatedTable.getTableRef();
	int numRow = evaluatedTable.getNumRow();
	std::list<std::string> resList;

	// 1. Projecting only selected columns
	// E.g.
	// { {STMT, {1, 3, 4}, {ASGN, {2, 2, 3}} },
	// Forms {"1, 2", "3, 2", "4, 3"} or 1 2 3 2 4 3 according to AutoTester
	for (int i = 0; i < numRow; i++) {	// for each row
		std::unordered_map<std::string, std::vector<int>>::iterator it = table.begin();
		std::string res = "";
		while (it != table.end()) {	// for each col
			std::string entityName = it->first;
			std::string value;
			// if column is not selected, do not project
			if (std::find(columnsProjected.begin(), columnsProjected.end(), entityName) == columnsProjected.end()) {
				it++;
				continue;
			}

			if (it->second.size() == 0) {
				break;
			}
			if (entities[entityName] == PqlEntityType::Stmt || entities[entityName] == PqlEntityType::Constant) {
				value = std::to_string(it->second[i]);
			}
			else if ((entities[entityName] == PqlEntityType::Variable)) {
				value = Entity::getVarName(it->second[i]);
			}
			else { //PqlEntityType::Procedure
				value = Entity::getProcName(it->second[i]);
			}

			if (res == "") {
				res = value;
			}
			else {
				res += " " + value;
			}

			it++;
		}
		resList.push_back(res);
	}

	// 2. Filter to unique values
	resList.unique();

	return resList;
}

PQLResultProjector::PQLResultProjector(EvaluatedTable evTable, std::vector<std::string> columns) :
	evaluatedTable(evTable),
	columnsProjected(columns) {}

PQLResultProjector::PQLResultProjector() {
}
