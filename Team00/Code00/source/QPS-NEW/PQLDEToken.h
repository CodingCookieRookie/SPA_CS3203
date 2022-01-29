#pragma once

using namespace std;

#include <iostream>
#include <string>
#include "DesignEntity.h"

class PQLDEToken
{
private:
	string synonym;
	string designEntity;

public:
    string getPQLDETokenString() {
        return "Entity: " + designEntity + " synonym: " + synonym;
    }

    void setDesignEntity(string de) {
        designEntity = de;
    }

    void setSynonym(string s) {
        synonym = s;
    }

    static PQLDEToken getPQLDEToken(string deString)
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
	
};