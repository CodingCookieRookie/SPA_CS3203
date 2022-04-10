#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "QPSCommons.h"

/* Helper class to hash a vector of integers */
class IntVectorHasher {
private:
	static const std::hash<int> hasher;
public:
	size_t operator()(const std::vector<int>& vi) const {
		size_t result = 0;
		/* Hash function taken from:
		https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
		which itself is taken from the Boost library */
		for (const int& i : vi) {
			result ^= i + 0x9e3779b9 + (result << 6) + (result >> 2);
		}
		return result;
	}
};

class EvaluatedTable {
private:
	std::unordered_map<std::string, std::vector<int>> table;
	bool evResult;
	void removeDuplicates();
	EvaluatedTable hashJoin(EvaluatedTable& otherTable,
		std::unordered_set<std::string>& commonEntities);
	void EvaluatedTable::prepopulate(std::unordered_map<std::string, std::vector<int>>& resultTable,
		const std::vector<std::string>& cols);

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

	/* Wrapper constructor for table only */
	EvaluatedTable(std::unordered_map<std::string, std::vector<int>> newTable);

	/* Wrapper constructor for boolean only (i.e. when the result evaluates to only a boolean) */
	EvaluatedTable(bool evResult);

	/* Handle table joins */
	EvaluatedTable innerJoinMerge(EvaluatedTable& otherTable);

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
	std::unordered_map<std::string, std::vector<int>>& getTableRef() {
		return table;
	}

	/* Getter for table */
	bool getEvResult() {
		return evResult;
	}

	EvaluatedTable project(const std::unordered_set<std::string>& columns);

	/* Mentions all the relevant fields of the EvalautedTable */
	std::string toString() {
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

	bool columnExists(const std::string& column);
};
