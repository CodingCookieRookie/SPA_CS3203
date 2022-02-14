#pragma once
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include "QPSCommons.h"

/* Defining different strings */
typedef std::string VALUE;

class EvaluatedTable {
private:
    std::unordered_map<std::string, PqlEntityType> entities;
    std::unordered_map<std::string, std::vector<int>> table;
    int numRow;

public:
    /* E.g. of an EvalauatedTable:
    * {"s", "v"} = {{"1", "a"}, {"2", "b"}}
    * 
    * EvalautedTable.entities == {"s", "v"} 
    * EvalautedTable.*table == {
        {"s", {"1", "2"}},
    *	{"v", {"a", "b"}}
    * }
    * numRow == 2
    */

    /* Default constructor */
    EvaluatedTable();

    /* Wrapper constructor */
    //EvaluatedTable(EvaluatedTable anotherEvTable);

    /** Wrapper constructor that constructs based on *table only 
    * 
    * e.g.
    * Given this from the PKB:
    * {"s", "v"} = {{"1", "a"}, {"2", "b"}}
    * 
    * We construct this:
    * EvaluatedTable(new std::unordered_map<PqlEntityType, std::vector<VALUE>>({
    *	{"s", {"1", "2"}},
    *	{"v", {"a", "b"}}
    * }))
    */
    EvaluatedTable(std::unordered_map<std::string, std::vector<int>> newTable) {
        table = newTable;
    }

    /* Wrapper constructor for all 3 fields */
    EvaluatedTable::EvaluatedTable(std::unordered_map<std::string, PqlEntityType> newEntities,
        std::unordered_map<std::string, std::vector<int>> newTable, int newNumRow);

    /* Getter for entities */
    std::unordered_map<std::string, PqlEntityType> getEntities() {
        return entities;
    }

    /* Getter for numRow */
    int getNumRow() {
        int numRows = 0;
        for (auto& it : table) { // for each col in original
            numRows = it.second.size();
        }
        return numRows;
    }

    /* Getter for *table */
    std::unordered_map<std::string, std::vector<int>> getTableRef() {
        return table;
    }

    std::string getTableString() {
        std::string res = "Table String: size: " + std::to_string(table.size()) + "\n";
        for (auto& it : table) {
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
};
