#include "EvaluatedTable.h"

/*
EvaluatedTable::EvaluatedTable() {
    numRow = 0;
}
*/

EvaluatedTable::EvaluatedTable() {
    entities = std::unordered_map<std::string, PqlEntityType>(); 
    table = std::unordered_map<std::string, std::vector<int>>();
    isEmpty = true;
    numRow = table.size();
}

EvaluatedTable::EvaluatedTable(std::unordered_map<std::string, std::vector<int>> table) :
    table(table),
    numRow(table.size()),
    isEmpty(false)
    {}

EvaluatedTable::EvaluatedTable(
    std::unordered_map<std::string, PqlEntityType> newEntities,
    std::unordered_map<std::string, std::vector<int>> newTable) :
    entities(newEntities),
    table(newTable),
    numRow(newTable.size()),
    isEmpty(false)
    {}


EvaluatedTable::EvaluatedTable(bool isEmpty) : isEmpty(isEmpty), numRow(0) {}
