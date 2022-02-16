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

    for (int i = 0; i < numRow; i++) {
        for (int j = 0; j < otherTable.numRow; j++) {
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

            nextNumRow++;
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
    return EvaluatedTable(nextEntities, nextTable, nextNumRow);
}

EvaluatedTable::EvaluatedTable() {
    entities = std::unordered_map<std::string, PqlEntityType>(); 
    table = std::unordered_map<std::string, std::vector<int>>();
    evResult = true;
    numRow = table.size();
}

/* Modifies the table in place, buy joining it with otherTable */
EvaluatedTable EvaluatedTable::innerJoinMerge(EvaluatedTable& otherTable) {
    /* Handle the case when we perform the first merge, which is done on a table with no rows */
    if (entities.empty()) {
        return EvaluatedTable(otherTable);
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

EvaluatedTable::EvaluatedTable(std::unordered_map<std::string, std::vector<int>> table) :
    table(table),
    numRow(table.size()),
    evResult(false)
    {}

EvaluatedTable::EvaluatedTable(
    std::unordered_map<std::string, PqlEntityType> newEntities,
    std::unordered_map<std::string, std::vector<int>> newTable) :
    entities(newEntities),
    table(newTable),
    numRow(newTable.size()),
    evResult(false)
    {}


EvaluatedTable::EvaluatedTable(bool evResult) : evResult(evResult), numRow(0) {}
