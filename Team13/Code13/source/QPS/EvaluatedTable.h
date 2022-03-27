#pragma once

#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "QPSCommons.h"

class EvaluatedTable {
private:
	std::unordered_map<std::string, EntityType> entities;
	std::unordered_map<std::string, std::vector<int>> table;
	bool evResult;

	void removeDuplicates();
	EvaluatedTable blockNestedJoin(EvaluatedTable& otherTable,
		std::unordered_set<std::string>& commonEntities);
	void EvaluatedTable::prepopulate(std::unordered_map<std::string, std::vector<int>>& nextTable,
		std::unordered_map<std::string, EntityType>& nextEntities,
		std::unordered_map<std::string, std::vector<int>>& currTable,
		std::unordered_map<std::string, EntityType>& currEntities);

public:
	/* E.g. of an EvalauatedTable:
	* {"s", "v"} = {{"1", "a"}, {"2", "b"}}
	*
	* EvalautedTable.entities == {
		{"s", Stmt},
		{"v", Variable}
	  }
	* EvalautedTable.table == {
		{"s", {"1", "2"}},
	*	{"v", {"a", "b"}}
	* }
	* numRow == 2
	*/

	/* Dummy default constructor */
	EvaluatedTable();

	/* Wrapper constructor for 2 fields, less boolean */
	EvaluatedTable(std::unordered_map<std::string, EntityType> newEntities,
		std::unordered_map<std::string, std::vector<int>> newTable);

	/* Wrapper constructor for boolean only (i.e. when the result evaluates to only a boolean) */
	EvaluatedTable(bool evResult);

	/* Handle table joins */
	EvaluatedTable innerJoinMerge(EvaluatedTable& otherTable);

	/* Checks if table field has no values */
	static bool EvaluatedTable::isNoValuesInResultTable(
		std::unordered_map<std::string, std::vector<int>> resultTable);

	/* Getter for entities */
	std::unordered_map<std::string, EntityType> getEntities() {
		return entities;
	}

	/* Getter for numRow */
	size_t getNumRow() {
		if (table.empty()) {
			return 0;
		}
		std::unordered_map<std::string, std::vector<int>>::iterator firstCol = table.begin();
		const std::vector<int>& firstColVector = firstCol->second;
		return firstColVector.size();
	}

	/* Getter for table */
	std::unordered_map<std::string, std::vector<int>> getTableRef() {
		return table;
	}

	/* Getter for table */
	bool getEvResult() {
		return evResult;
	}

	/* Mentions all the relevant fields of the EvalautedTable */
	std::string getTableString() {
		std::map<std::string, std::vector<int>> ordered(table.begin(), table.end());
		std::string res = "Table String: size: " + std::to_string(ordered.size()) + "\n";
		for (auto& it : ordered) {
			res += "Synonym: " + it.first + " ";
			res += "Values: ";
			std::vector<int> values = it.second;
			for (size_t i = 0; i < values.size(); i++) {
				res = res + std::to_string(values.at(i)) + " ";
			}
			res += "\n";
		}
		return res;
	}
};
