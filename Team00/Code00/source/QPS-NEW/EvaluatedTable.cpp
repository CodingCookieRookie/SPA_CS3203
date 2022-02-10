#include "EvaluatedTable.h"

/*
EvaluatedTable::EvaluatedTable() {
    numRow = 0;
}
*/

EvaluatedTable::EvaluatedTable() {
    entities = std::unordered_map<std::string, PqlEntityType>(); 
    table = new std::unordered_map<std::string, std::vector<VALUE>>();
    numRow = 0;
}

EvaluatedTable::EvaluatedTable(const EvaluatedTable& anotherEvTable) {
    entities = anotherEvTable.entities;
    numRow = anotherEvTable.numRow;
    //table = new std::unordered_map<std::string, std::vector<VALUE>>();
    *table = *anotherEvTable.table;
}

//EvaluatedTable::EvaluatedTable(std::unordered_map<std::string, std::vector<VALUE>>* anotherTable) {
//    int numRows = anotherTable->size() != 0 ? (*anotherTable->begin()).second.size() : 0;
//    for (const std::pair< std::string, std::vector<VALUE>> kvPair : *anotherTable) {
//        //entities[kvPair.first] = getType()..
//    }
//    *table = *anotherTable;
//}

//EvaluatedTable::EvaluatedTable(std::unordered_map<std::string, PqlEntityType> newEntities,
//    std::unordered_map<std::string, std::vector<VALUE>>& newTable, int newNumRow ) {
//
//    entities = newEntities;
//    table = &newTable;
//    numRow = newNumRow;
//}
