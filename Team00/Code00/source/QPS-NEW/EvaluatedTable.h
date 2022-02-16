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
    bool evResult; //if evaluated to false
    int numRow;

public:
    /* E.g. of an EvalauatedTable:
    * {"s", "v"} = {{"1", "a"}, {"2", "b"}}
    * 
    * EvalautedTable.entities == {
        {"s", Stmt},
        {"v", Variable} 
      }
    * EvalautedTable.table == {
        {"s", {"1", "2"}},
    *	{"v", {"a", "b"}}
    * }
    * numRow == 2
    */

    /* Default constructor */
    EvaluatedTable();

    EvaluatedTable(std::unordered_map<std::string, std::vector<int>> table);

    /* Wrapper constructor for 2 fields, less boolean and numRow */
    EvaluatedTable(std::unordered_map<std::string, PqlEntityType> newEntities,
        std::unordered_map<std::string, std::vector<int>> newTable);

    /* Wrapper constructor for boolean only (i.e. when the result evaluates to only a boolean) */
    EvaluatedTable(bool evResult);

    /* Getter for entities */
    std::unordered_map<std::string, PqlEntityType> getEntities() {
        return entities;
    }

    /* Getter for numRow */
    int getNumRow() {
        return numRow;
    }

    /* Getter for table */
    std::unordered_map<std::string, std::vector<int>> getTableRef() {
        return table;
    }

    /* Getter for table */
    bool getEvResult() {
        return evResult;
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
