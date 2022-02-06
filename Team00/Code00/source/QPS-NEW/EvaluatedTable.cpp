#include "EvaluatedTable.h"

EvaluatedTable::EvaluatedTable(const EvaluatedTable& anotherEvTable) {
	entities = anotherEvTable.entities;
	numRow = anotherEvTable.numRow;
	table = new std::unordered_map<PQL_VARIABLE_TYPE, std::vector<VALUE>>();
	*table = *anotherEvTable.table
}

EvaluatedTable::EvaluatedTable(std::unordered_map<PQL_VARIABLE_TYPE, std::vector<VALUE>>* anotherTable) {
	numRows = anotherTable->size() != 0 ? (*anotherTable->begin()).second.size() : 0;
	for (const std::pair< PQL_VARIABLE_TYPE, std::vector<VALUE>> kvPair : anotherTable) {
		entities.insert(kvPair.first);
	}
	*table = *anotherTable;
}
