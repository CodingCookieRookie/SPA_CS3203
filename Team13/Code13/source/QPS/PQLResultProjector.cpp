#include "PQLResultProjector.h"
#include "EvaluatedTable.h"
#include "../PKB/Entity.h"
#include "QPSCommons.h"
#include "../QPS/Instruction.h"

#include <string>
#include <list>
#include <unordered_set>

std::list<std::string> PQLResultProjector::resolveTableToResults() {
	std::unordered_map<std::string, std::vector<int>> table = evaluatedTable.getTableRef();
	std::list<std::string> resList;

	std::unordered_map<std::string, PqlEntityType> resultEntities;
	std::unordered_map<std::string, std::vector<int>> resultTable;
	for (std::string& column : columnsProjected) {
		/* For each column that already exists in the final EvTable, take it from the EvTable */
		if (table.find(column) != table.end()) {
			resultEntities[column] = declarations[column];
			resultTable[column] = table[column];
		}
	}

	/* If the evaluated table is false or an empty table, use a false table */
	EvaluatedTable resultEvTable(resultEntities, resultTable);
	if (evaluatedTable.getEvResult() == false
		|| (table.size() > 0 && evaluatedTable.getNumRow() == 0)) {
		resultEvTable = EvaluatedTable(false);
	}

	for (std::string& column : columnsProjected) {
		if (table.find(column) == table.end()) {
			/* For each column that does not exist in the final EvTable, get it via an instruction
			   and then perform a cross product. */
			PqlEntityType columnType = declarations.at(column);
			Instruction* getAll = new GetAllInstruction(columnType, column);
			EvaluatedTable evTable = getAll->execute();
			resultEvTable = resultEvTable.innerJoinMerge(evTable);
		}
	}
	/* Extract the table from the EvTable */
	resultTable = resultEvTable.getTableRef();
	int numRow = resultEvTable.getNumRow();

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

PQLResultProjector::PQLResultProjector(
	EvaluatedTable evTable,
	std::vector<std::string> columns,
	std::unordered_map<std::string, PqlEntityType> declarations) :
	evaluatedTable(evTable),
	columnsProjected(columns),
	declarations(declarations) {}

PQLResultProjector::PQLResultProjector() {}
