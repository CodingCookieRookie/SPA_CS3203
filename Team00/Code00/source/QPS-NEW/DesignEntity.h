using namespace std;

#include <iostream>
#include <string>
#include <unordered_map>
#pragma once



const static unordered_map<string, int> designEntities = {
            { "stmt", 0 },
            { "read", 1 },
            { "print", 2 }, 
            { "call", 3 },
            { "while", 4 },
            { "if", 5 },
            { "assign", 6 },
            { "variable", 7 },
            { "constant", 8 },
            { "procedure", 9 },
};

static bool checkIfWordIsDesignEntity(string word) {
    return designEntities.find(word) != designEntities.end();
}
