#include "EvaluatedTable.h"

/*
EvaluatedTable::EvaluatedTable() {
    numRow = 0;
}
*/

EvaluatedTable::EvaluatedTable() {
    entities = std::unordered_set<PqlEntityType>();
    table = new std::unordered_map<PqlEntityType, std::vector<VALUE>>();
    numRow = 0;
}

EvaluatedTable::EvaluatedTable(const EvaluatedTable& anotherEvTable) {
    entities = anotherEvTable.entities;
    numRow = anotherEvTable.numRow;
    table = new std::unordered_map<PqlEntityType, std::vector<VALUE>>();
    *table = *anotherEvTable.table;
}

EvaluatedTable::EvaluatedTable(std::unordered_map<PqlEntityType, std::vector<VALUE>>* anotherTable) {
    int numRows = anotherTable->size() != 0 ? (*anotherTable->begin()).second.size() : 0;
    for (const std::pair< PqlEntityType, std::vector<VALUE>> kvPair : *anotherTable) {
        entities.insert(kvPair.first);
    }
    *table = *anotherTable;
}

EvaluatedTable::EvaluatedTable(std::unordered_set<PqlEntityType> newEntities,
    std::unordered_map<PqlEntityType, std::vector<VALUE>>& newTable, int newNumRow ) {

    entities = newEntities;
    table = &newTable;
    numRow = newNumRow;
}


