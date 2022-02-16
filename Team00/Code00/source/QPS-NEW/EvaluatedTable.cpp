#include "EvaluatedTable.h"

/*
EvaluatedTable::EvaluatedTable() {
    numRow = 0;
}
*/

EvaluatedTable::EvaluatedTable() {
    entities = std::unordered_map<std::string, PqlEntityType>(); 
    table = std::unordered_map<std::string, std::vector<int>>();
    evResult = true;
    numRow = table.size();
}

EvaluatedTable::EvaluatedTable(std::unordered_map<std::string, std::vector<int>> table) :
    table(table),
    numRow(table.size()),
    evResult(false)
    {}

EvaluatedTable::EvaluatedTable(std::unordered_map<std::string, PqlEntityType> entities, std::unordered_map<std::string, std::vector<int>> table) :
    entities(entities),
    table(table)
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
