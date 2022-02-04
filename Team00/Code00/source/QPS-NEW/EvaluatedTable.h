#pragma once
#include <vector>
#include <string>

#include <unordered_set>
#include <unordered_map>

/* Defining different strings */
typedef std::string ENTITY, VALUE;

class EvaluatedTable {
private:
	std::unordered_set<ENTITY> entities;
	std::unordered_map<ENTITY, std::vector<VALUE>> table;
	int numRow;

public:

	/* Default constructor */
	EvaluatedTable();

	/* Wrapper constructor */
	EvaluatedTable(const EvaluatedTable& anotherEvaluatedTable);

};