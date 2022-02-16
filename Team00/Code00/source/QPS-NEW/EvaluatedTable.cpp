#include "EvaluatedTable.h"

/*
EvaluatedTable::EvaluatedTable() {
    numRow = 0;
}
*/

EvaluatedTable::EvaluatedTable() {
    entities = std::unordered_map<std::string, PqlEntityType>(); 
    table = std::unordered_map<std::string, std::vector<int>>();
    numRow = 0;
}

EvaluatedTable::EvaluatedTable(std::unordered_map<std::string, std::vector<int>> table) :
    table(table),
    numRow(table.size())
    {}

EvaluatedTable::EvaluatedTable(
    std::unordered_map<std::string, PqlEntityType> newEntities,
    std::unordered_map<std::string, std::vector<int>> newTable, 
    int newNumRow ) {
    entities = newEntities;
    table = newTable;
    numRow = newNumRow;
}
