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
	std::unordered_map<PQL_VARIABLE_TYPE, std::vector<VALUE>> table;
	int numRow;

public:

	/* Default constructor */
	EvaluatedTable();

	/* Wrapper constructor */
	EvaluatedTable(const EvaluatedTable& anotherEvaluatedTable);

};