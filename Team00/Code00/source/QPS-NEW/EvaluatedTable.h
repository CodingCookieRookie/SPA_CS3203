#pragma once
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include "QPSCommons.h"

/* Defining different strings */
typedef std::string VALUE;

class EvaluatedTable {
private:
	std::unordered_set<PQL_VARIABLE_TYPE> entities;
	std::unordered_map<PQL_VARIABLE_TYPE, std::vector<VALUE>>* table;
	int numRow;

public:
	/* E.g. of an EvalauatedTable:
	* {"s", "v"} = {{"1", "a"}, {"2", "b"}}
	* 
	* EvalautedTable.entities == {"s", "v"} 
	* EvalautedTable.*table == {
		{"s", {"1", "2"}},
	*	{"v", {"a", "b"}}
	* }
	* numRow == 2
	*/

	/* Default constructor */
	EvaluatedTable();

	/* Wrapper constructor */
	EvaluatedTable(const EvaluatedTable& anotherEvTable);

	/** Wrapper constructor that constructs based on *table only 
	* 
	* e.g.
	* Given this from the PKB:
	* {"s", "v"} = {{"1", "a"}, {"2", "b"}}
	* 
	* We construct this:
	* EvaluatedTable(new std::unordered_map<PQL_VARIABLE_TYPE, std::vector<VALUE>>({
	*	{"s", {"1", "2"}},
	*	{"v", {"a", "b"}}
	* }))
	*/
	EvaluatedTable(std::unordered_map<PQL_VARIABLE_TYPE, std::vector<VALUE>> *table);

	/* Wrapper constructor for all 3 fields */
	EvaluatedTable::EvaluatedTable(std::unordered_set<PQL_VARIABLE_TYPE> newEntities,
		std::unordered_map<PQL_VARIABLE_TYPE, std::vector<VALUE>>& newTable, int newNumRow)

	/* Getter for entities */
	std::unordered_set<PQL_VARIABLE_TYPE> getEntities() {
		return entities;
	}

	/* Getter for numRow */
	int getNumRow() {
		return numRow;
	}

	/* Getter for *table */
	std::unordered_map<PQL_VARIABLE_TYPE, std::vector<VALUE>>* getTableRef() {
		return table;
	}
};