using namespace std;

#include <iostream>
#include <string>
#include <unordered_map>
#pragma once



const static unordered_map<string, int> designEntities = {
            { "stmt", 0 },
            { "read", 1 },
            { "print", 2 }, 
};

static bool checkIfWordIsDesignEntity(string word) {
    return designEntities.find(word) != designEntities.end();
}
