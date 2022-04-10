#include "EvaluatedTable.h"

const std::hash<int> IntVectorHasher::hasher{};

void EvaluatedTable::removeDuplicates() {
	/* If the table contains only one column, this function removes all duplicates in that column.
	   Otherwise, the table is left unmodified */
	if (table.size() > 1) {
		return;
	}
	std::unordered_set<int> uniqueVals;
	Table uniqueTable;
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

void EvaluatedTable::prepopulate(Table& resultTable,
	const std::vector<std::string>& cols) {
	for (const std::string& col : cols) {
		resultTable.insert({ col, std::vector<int>() });
	}
}

Table EvaluatedTable::filterUnique(const Table& originalTable) {
	std::unordered_map<std::string, std::vector<int>> nextTable;
	if (originalTable.empty()) {
		return originalTable;
	}
	size_t numColumns = originalTable.size();
	std::vector<std::string> columns;
	columns.reserve(numColumns);
	for (const auto& [column, values] : originalTable) {
		columns.push_back(column);
		nextTable.insert({ column, std::vector<int>() });
	}

	size_t numRows = originalTable.begin()->second.size();
	std::unordered_set<std::vector<int>, IntVectorHasher> uniqueRows;
	uniqueRows.reserve(numRows);
	for (size_t i = 0; i < numRows; i++) {
		std::vector<int> row(numColumns);
		std::transform(columns.begin(), columns.end(),
			row.begin(), [&originalTable, i](const std::string& colName) { return originalTable.at(colName).at(i); });
		uniqueRows.insert(row);
	}

	for (const std::vector<int>& row : uniqueRows) {
		for (size_t i = 0; i < numColumns; i++) {
			std::string& column = columns[i];
			int value = row[i];
			nextTable.at(column).push_back(value);
		}
	}

	return nextTable;
}

std::vector<std::string> EvaluatedTable::getUniqueColumns(const Table& tableVal, std::unordered_set<std::string>& commonEntities) {
	/* Gets unique columns from the left table */
	std::vector<std::string> columns;
	for (const auto& [column, value] : tableVal) {
		if (commonEntities.find(column) == commonEntities.end()) {
			columns.push_back(column);
		}
	}
	return columns;
}

std::unordered_map<std::vector<int>, std::vector<std::vector<int>>, IntVectorHasher> EvaluatedTable::buildHashMap(
	const Table& tableVal,
	const std::vector<std::string>& commonColumns,
	const std::vector<std::string>& uniqueColumns,
	size_t numRows) {
	std::unordered_map<std::vector<int>, std::vector<std::vector<int>>, IntVectorHasher> tableHashMap;
	size_t numCommon = commonColumns.size();
	size_t numUnique = uniqueColumns.size();
	
	/* Convert the table into a HashMap mapping common cols -> rows */
	for (size_t i = 0; i < numRows; i++) {
		/* Key refers to the indexes of the common columns between both tables */
		/* Note that we need to pre-allocate this size of the mapped vector,
		since std::transform will not expand the vector for us */
		std::vector<int> key(numCommon);
		std::transform(commonColumns.begin(), commonColumns.end(),
			key.begin(), [&tableVal, i](const std::string& colName) { return tableVal.at(colName).at(i); });
		/* Value refers to the indexes of the columns in table that are not in otherTable */
		std::vector<int> value(numUnique);
		std::transform(uniqueColumns.begin(), uniqueColumns.end(),
			value.begin(), [&tableVal, i](const std::string& colName) { return tableVal.at(colName).at(i); });
		tableHashMap[key].push_back(value);
	}
	return tableHashMap;
}

Table EvaluatedTable::probeStep(const std::unordered_map<std::vector<int>, std::vector<std::vector<int>>, IntVectorHasher>& tableHashMap,
	const std::unordered_map<std::vector<int>, std::vector<std::vector<int>>, IntVectorHasher>& otherHashMap,
	const std::vector<std::string>& commonColumns,
	const std::vector<std::string>& tableColumns,
	const std::vector<std::string>& otherColumns) {
	Table resultTable;

	size_t numCommon = commonColumns.size();
	size_t numTable = tableColumns.size();
	size_t numOther = otherColumns.size();

	/* Prepopulate the columns of the resulting table */
	prepopulate(resultTable, commonColumns);
	prepopulate(resultTable, tableColumns);
	prepopulate(resultTable, otherColumns);

	/* Perform the join/probe step */
	for (const auto& [key, tableValue] : tableHashMap) {
		if (otherHashMap.find(key) == otherHashMap.end()) {
			continue;
		}
		const std::vector<std::vector<int>>& otherValue = otherHashMap.at(key);
		for (const std::vector<int>& tableRow : tableValue) {
			for (const std::vector<int>& otherRow : otherValue) {
				/* Populate table with common cols */
				for (size_t i = 0; i < numCommon; i++) {
					resultTable[commonColumns.at(i)].push_back(key.at(i));
				}
				/* Populate table with cols from table */
				for (size_t i = 0; i < numTable; i++) {
					resultTable[tableColumns.at(i)].push_back(tableRow.at(i));
				}
				/* Populate table with cols from otherTable */
				for (size_t i = 0; i < numOther; i++) {
					resultTable[otherColumns.at(i)].push_back(otherRow.at(i));
				}
			}
		}
	}
	return resultTable;
}

EvaluatedTable EvaluatedTable::hashJoin(EvaluatedTable& otherTable, std::unordered_set<std::string>& commonEntities) {
	size_t numRows = getNumRow();
	size_t otherRows = otherTable.getNumRow();

	/* Store a list of common columns */
	std::vector<std::string> commonColumns(commonEntities.begin(), commonEntities.end());

	/* Store a list of columns unique to table */
	std::vector<std::string> tableColumns = getUniqueColumns(table, commonEntities);

	/* Store a list of columns unique to otherTable */
	std::vector<std::string> otherColumns = getUniqueColumns(otherTable.table, commonEntities);

	/* Build Step */
	std::unordered_map<std::vector<int>, std::vector<std::vector<int>>, IntVectorHasher> tableHashMap = 
		buildHashMap(table, commonColumns, tableColumns, numRows);
	std::unordered_map<std::vector<int>, std::vector<std::vector<int>>, IntVectorHasher> otherHashMap =
		buildHashMap(otherTable.table, commonColumns, otherColumns, otherRows);

	/* Probe step */
	Table resultTable = probeStep(tableHashMap, otherHashMap, commonColumns, tableColumns, otherColumns);
	
	return EvaluatedTable(resultTable);
}

/* Joins the current table with otherTable, returning a new EvaluatedTable object */
EvaluatedTable EvaluatedTable::innerJoinMerge(EvaluatedTable& otherTable) {
	/* If either table is "false", the joined table is also "false" */
	if (!evResult || !otherTable.evResult) {
		return EvaluatedTable(false);
	}
	/* If this table is "true", return the other table */
	if (table.size() == 0) {
		return EvaluatedTable(otherTable.table);
	}
	/* Likewise, if otherTable is "true", return this table */
	if (otherTable.table.size() == 0) {
		return EvaluatedTable(table);
	}
	std::unordered_set<std::string> commonEntities;
	for (const auto& [synonym, values] : table) {
		if (otherTable.table.find(synonym) != otherTable.table.end()) {
			commonEntities.insert(synonym);
		}
	}
	return hashJoin(otherTable, commonEntities);
}

EvaluatedTable EvaluatedTable::project(const std::unordered_set<std::string>& columns) {
	if (table.empty()) {
		return EvaluatedTable(evResult);
	}
	std::unordered_map<std::string, std::vector<int>> nextTable;
	for (const std::string& column : columns) {
		if (table.find(column) != table.end()) {
			nextTable[column] = table.at(column);
		}
	}
	/* No columns projected, return a true/false evTable */
	if (nextTable.empty()) {
		return EvaluatedTable(getNumRow() > 0);
	}
	std::unordered_map<std::string, std::vector<int>> filteredTable = filterUnique(nextTable);
	return EvaluatedTable(filteredTable);
}

EvaluatedTable::EvaluatedTable() : EvaluatedTable(true) {}

EvaluatedTable::EvaluatedTable(Table newTable) :
	table(newTable),
	evResult(true) {}

EvaluatedTable::EvaluatedTable(bool evResult) :
	table(),
	evResult(evResult) {}

bool EvaluatedTable::columnExists(const std::string& column) {
	return table.find(column) != table.end();
}

std::string EvaluatedTable::toString() {
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

size_t EvaluatedTable::getNumRow() {
	if (table.empty()) {
		return 0;
	}
	Table::iterator firstCol = table.begin();
	const std::vector<int>& firstColVector = firstCol->second;
	return firstColVector.size();
}

Table& EvaluatedTable::getTableRef() {
	return table;
}

bool EvaluatedTable::getEvResult() {
	return evResult;
}

int EvaluatedTable::getCell(const std::string& column, int row) {
	return table.at(column).at(row);
}
