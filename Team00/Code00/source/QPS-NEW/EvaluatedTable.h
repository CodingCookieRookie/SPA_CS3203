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

    EvaluatedTable blockNestedJoin(EvaluatedTable& otherTable,
        std::unordered_set<std::string>& commonEntities);

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

    /* Dummy default constructor -- to remove if we can better handle this */
    EvaluatedTable();

    /* Wrapper constructor for 2 fields, less boolean */
    EvaluatedTable(std::unordered_map<std::string, PqlEntityType> newEntities,
        std::unordered_map<std::string, std::vector<int>> newTable);

    /* Wrapper constructor for boolean only (i.e. when the result evaluates to only a boolean) */
    EvaluatedTable(bool evResult);

    /* Handle table joins */
    EvaluatedTable innerJoinMerge(EvaluatedTable& otherTable);

    /* Getter for entities */
    std::unordered_map<std::string, PqlEntityType> getEntities() {
        return entities;
    }

    /* Getter for numRow */
    size_t getNumRow() {
        if (table.empty()) {
            return 0;
        }
        std::unordered_map<std::string, std::vector<int>>::iterator firstCol = table.begin();
        const std::vector<int>& firstColVector = firstCol->second;
        return firstColVector.size();
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