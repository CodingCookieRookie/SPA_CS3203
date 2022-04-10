#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../QPSCommons.h"

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
	Table table;
	bool evResult;
	void removeDuplicates();
	EvaluatedTable hashJoin(EvaluatedTable& otherTable,
		std::unordered_set<std::string>& commonEntities);
	void EvaluatedTable::prepopulate(Table& resultTable,
		const std::vector<std::string>& cols);
	Table filterUnique(const Table& originalTable);
	std::vector<std::string> getUniqueColumns(const Table& tableVal, std::unordered_set<std::string>& commonEntities);
	std::unordered_map<std::vector<int>, std::vector<std::vector<int>>, IntVectorHasher> buildHashMap(
		const Table& tableVal,
		const std::vector<std::string>& commonColumns,
		const std::vector<std::string>& uniqueColumns,
		size_t numRows);
	Table probeStep(const std::unordered_map<std::vector<int>, std::vector<std::vector<int>>, IntVectorHasher>& tableHashMap,
		const std::unordered_map<std::vector<int>, std::vector<std::vector<int>>, IntVectorHasher>& otherHashMap,
		const std::vector<std::string>& commonColumns,
		const std::vector<std::string>& tableColumns,
		const std::vector<std::string>& otherColumns);
public:
	EvaluatedTable();
	EvaluatedTable(Table newTable);
	/* Wrapper constructor for boolean only (i.e. when the result evaluates to only a boolean) */
	EvaluatedTable(bool evResult);

	/* Method to handle table joins using hash join, an optimised joining algorithm */
	EvaluatedTable innerJoinMerge(EvaluatedTable& otherTable);

	/* Method to filter down columns in table to projected columns  */
	EvaluatedTable project(const std::unordered_set<std::string>& columns);

	/* Mentions all the relevant fields of the EvalautedTable */
	std::string toString();

	bool columnExists(const std::string& column);

	/* Getters */
	size_t getNumRow();

	Table& getTableRef();

	bool getEvResult();

	int getCell(const std::string& column, int row);
};
