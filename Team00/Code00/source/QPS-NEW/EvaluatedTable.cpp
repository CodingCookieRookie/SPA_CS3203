#include "EvaluatedTable.h"

EvaluatedTable EvaluatedTable::blockNestedJoin(EvaluatedTable& otherTable,
	std::unordered_set<std::string>& commonEntities) {
	std::unordered_map<std::string, std::vector<int>> nextTable;
	std::unordered_map<std::string, PqlEntityType> nextEntities;
	int nextNumRow = 0;
	/* Populate the columns of the new table */
	for (const std::pair<std::string, PqlEntityType>& taggedEntity : entities) {
		std::string entityName = taggedEntity.first;
		nextTable[entityName] = std::vector<int>();
		nextEntities.insert(taggedEntity);
	}
	for (const std::pair<std::string, PqlEntityType>& taggedEntity : otherTable.entities) {
		std::string entityName = taggedEntity.first;
		if (nextTable.find(entityName) == nextTable.end()) {
			nextTable[entityName] = std::vector<int>();
			nextEntities.insert(taggedEntity);
		}
	}

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
			for (const std::pair<std::string, PqlEntityType>& taggedEntity : entities) {
				std::string entityName = taggedEntity.first;
				nextTable[entityName].push_back(table[entityName][i]);
			}
			/* Add all values (except those in the common columns) from the right side table to the result table */
			for (const std::pair<std::string, PqlEntityType>& taggedEntity : otherTable.entities) {
				std::string entityName = taggedEntity.first;
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
	for (const std::pair<std::string, PqlEntityType>& taggedEntity : entities) {
		std::string entityName = taggedEntity.first;
		if (otherTable.entities.find(entityName) != otherTable.entities.end()) {
			commonEntities.insert(entityName);
		}
	}
	/* Perform an block nested join -- To update to a inner hash join where possible */
	return blockNestedJoin(otherTable, commonEntities);
}

EvaluatedTable::EvaluatedTable() : EvaluatedTable(true) { }

EvaluatedTable::EvaluatedTable(
	std::unordered_map<std::string, PqlEntityType> newEntities,
	std::unordered_map<std::string, std::vector<int>> newTable) :
	table(newTable),
	entities(newEntities),
	evResult(true)
{}

EvaluatedTable::EvaluatedTable(bool evResult) :
	table(),
	entities(),
	evResult(evResult)
{}