#pragma once

using namespace std;

#include "PQLDEToken.h"

//void PQLDEToken::setPQLDEToken(string userQuery)
//{
//    // check if exists in DesignEntity
//    // If yes create a PQLDEToken and get whatever that is behind to be synonym
//    // If no just return
//}
string PQLDEToken::getPQLDETokenString() {
    return "Entity: " + designEntity + " synonym: " + synonym;
}

void PQLDEToken::setDesignEntity(string de) {
    designEntity = de;
}

void PQLDEToken::setSynonym(string s) {
    synonym = s;
}

PQLDEToken PQLDEToken::getPQLDEToken(string deString)
{
    // check if exists in DesignEntity
    // If yes create a PQLDEToken and get whatever that is behind to be synonym
    // If no just return
    PQLDEToken deToken;
    size_t index = 0;
    size_t spaceIndex = 0;
    while (index >= 0) {
        index = deString.find_first_not_of(" ");
        cout << "index: " << index << "\n";
        spaceIndex = deString.find(" ");
        cout << "spaceIndex: " << spaceIndex << "\n";
        string word = deString.substr(index, spaceIndex);
        if (checkIfWordIsDesignEntity(word)) {   // means it is a designEntity
            deToken.setDesignEntity(word);
        } else {
            deToken.setSynonym(word);
        }
        if (spaceIndex < 0) break; // means that this is the last word
        deString = deString.substr(spaceIndex);
    }
    return deToken;
}