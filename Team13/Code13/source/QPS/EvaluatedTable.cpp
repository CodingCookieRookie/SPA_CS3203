#include "EvaluatedTable.h"

void EvaluatedTable::removeDuplicates() {
	/* If the table contains only one column, this function removes all duplicates in that column.
	   Otherwise, the table is left unmodified */
	if (table.size() > 1) {
		return;
	}
	std::unordered_set<int> uniqueVals;
	std::unordered_map<std::string, std::vector<int>> uniqueTable;
	for (const std::pair<std::string, std::vector<int>>& column : table) {
		const std::string& columnName = column.first;
		const std::vector<int>& columnVals = column.second;
		for (int columnVal : columnVals) {
			uniqueVals.insert(columnVal);
		}
		uniqueTable[columnName] = std::vector<int>();
		for (const int& uniqueVal : uniqueVals) {
			uniqueTable[columnName].push_back(uniqueVal);
		}
	}
	table = uniqueTable;
}

void EvaluatedTable::prepopulate(std::unordered_map<std::string, std::vector<int>>& nextTable,
	std::unordered_map<std::string, EntityType>& nextEntities,
	std::unordered_map<std::string, std::vector<int>>& currTable,
	std::unordered_map<std::string, EntityType>& currEntities) {
	for (const std::pair<std::string, std::vector<int>>& column : currTable) {
		std::string entityName = column.first;
		if (nextTable.find(entityName) == nextTable.end()) {
			nextTable[entityName] = std::vector<int>();
			nextEntities[entityName] = currEntities[entityName];
		}
	}
}

EvaluatedTable EvaluatedTable::blockNestedJoin(EvaluatedTable& otherTable,
	std::unordered_set<std::string>& commonEntities) {
	std::unordered_map<std::string, std::vector<int>> nextTable;
	std::unordered_map<std::string, EntityType> nextEntities;
	prepopulate(nextTable, nextEntities, table, entities);
	prepopulate(nextTable, nextEntities, otherTable.table, otherTable.entities);

	for (size_t i = 0; i < getNumRow(); i++) {
		for (size_t j = 0; j < otherTable.getNumRow(); j++) {
			/* Join two rows iff all their common columns have the same values */
			bool joinCondition = true;
			for (const std::string& entityName : commonEntities) {
				int leftSideValue = table[entityName][i];
				int rightSideValue = otherTable.table[entityName][j];
				if (leftSideValue != rightSideValue) {
					joinCondition = false;
					break;
				}
			}
			if (!joinCondition) {
				continue;
			}

			/* Add all values from the left side table to the result table */
			for (const std::pair<std::string, std::vector<int>>& column : table) {
				std::string entityName = column.first;
				nextTable[entityName].push_back(table[entityName][i]);
			}
			/* Add all values (except those in the common columns) from the right side table to the result table */
			for (const std::pair<std::string, std::vector<int>>& column : otherTable.table) {
				std::string entityName = column.first;
				if (commonEntities.find(entityName) != commonEntities.end()) {
					continue;
				}
				nextTable[entityName].push_back(otherTable.table[entityName][j]);
			}
		}
	}
	return EvaluatedTable(nextEntities, nextTable);
}

/* Joins the current table with otherTable, returning a new EvaluatedTable object */
EvaluatedTable EvaluatedTable::innerJoinMerge(EvaluatedTable& otherTable) {
	/* If either table is "false", the joined table is also "false" */
	if (!evResult || !otherTable.evResult) {
		return EvaluatedTable(false);
	}
	/* If this table is "true", return the other table */
	if (entities.size() == 0) {
		return EvaluatedTable(otherTable.entities, otherTable.table);
	}
	/* Likewise, if otherTable is "true", return this table */
	if (otherTable.entities.size() == 0) {
		return EvaluatedTable(entities, table);
	}
	std::unordered_set<std::string> commonEntities;
	for (const std::pair<std::string, EntityType>& taggedEntity : entities) {
		std::string entityName = taggedEntity.first;
		if (otherTable.entities.find(entityName) != otherTable.entities.end()) {
			commonEntities.insert(entityName);
		}
	}
	/* Perform an block nested join -- To update to a inner hash join where possible */
	return blockNestedJoin(otherTable, commonEntities);
}

bool EvaluatedTable::isNoValuesInResultTable(
	std::unordered_map<std::string, std::vector<int>> resultTable) {
	if (resultTable.empty()) {
		return true;
	}
	bool isEmpty = false;
	for (std::pair<std::string, std::vector<int>> row : resultTable) {
		if ((row.second.empty())) {
			isEmpty = true;
		}
	}
	return isEmpty;
}

EvaluatedTable::EvaluatedTable() : EvaluatedTable(true) {}

EvaluatedTable::EvaluatedTable(
	std::unordered_map<std::string, EntityType> newEntities,
	std::unordered_map<std::string, std::vector<int>> newTable) :
	table(newTable),
	entities(newEntities),
	evResult(true) {}

EvaluatedTable::EvaluatedTable(bool evResult) :
	table(),
	entities(),
	evResult(evResult) {}
