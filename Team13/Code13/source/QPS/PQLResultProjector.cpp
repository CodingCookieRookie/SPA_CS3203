#include <list>
#include <string>
#include <unordered_set>

#include "../PKB/Entity.h"
#include "../QPS/Instruction.h"
#include "EvaluatedTable.h"
#include "PQLResultProjector.h"
#include "QPSCommons.h"

std::list<std::string> PQLResultProjector::resolveTableToResults(
	EvaluatedTable evaluatedTable, std::vector<std::string> columnsProjected,
	std::unordered_map<std::string, PqlEntityType> declarations) {
	std::list<std::string> resList;
	std::unordered_map<std::string, std::vector<int>> resultTable = evaluatedTable.getTableRef();
	int numRow = evaluatedTable.getNumRow();

	// 1. Projecting only selected columns
	// E.g.
	// { {STMT, {1, 3, 4}, {ASGN, {2, 2, 3}} },
	// Forms {"1, 2", "3, 2", "4, 3"} or 1 2 3 2 4 3 according to AutoTester
	for (int i = 0; i < numRow; i++) {	// for each row
		std::string res = "";
		bool firstCol = true;
		for (std::string& entityName : columnsProjected) {
			std::string value = "";
			if (isStatementEntity(declarations[entityName])
				|| declarations[entityName] == PqlEntityType::Constant) {
				value = std::to_string(resultTable[entityName][i]);
			} else if ((declarations[entityName] == PqlEntityType::Variable)) {
				value = Entity::getVarName(resultTable[entityName][i]);
			} else { //PqlEntityType::Procedure
				value = Entity::getProcName(resultTable[entityName][i]);
			}
			if (!firstCol) {
				res += " ";
			}
			res += value;
			firstCol = false;
		}
		resList.push_back(res);
	}

	// 2. Filter to unique values
	resList.unique();

	return resList;
}
