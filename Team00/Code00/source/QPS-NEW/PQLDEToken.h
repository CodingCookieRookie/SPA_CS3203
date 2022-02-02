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
        bool expectDesignEntity = true;
        int count = 0;
        while (index != string::npos) {
            if (count >= 2) {   // invalid, for now count should have ended at 2 - one DE one var
                PQLDEToken newDEToken;
                return newDEToken;
            }
            index = deString.find_first_not_of(" ");
            cout << "index: " << index << "\n";
            spaceIndex = deString.find(" ");
            cout << "spaceIndex: " << spaceIndex << "\n";
            string word = deString.substr(index, spaceIndex);   // will always return a word starting from non space char to right before there is a space
            if (checkIfWordIsDesignEntity(word)) {   // means it is a designEntity
                if (!expectDesignEntity) return deToken;    // invalid
                deToken.setDesignEntity(word);
                expectDesignEntity = false;
            } else {
                if (expectDesignEntity) return deToken;     // invalid
                word.erase(remove(word.begin(), word.end(), ';'), word.end());  // remove ';' from synonyms
                deToken.setSynonym(word);
                expectDesignEntity = true;
            }
            count++;
            if (spaceIndex == string::npos) break; // means that this is the last word
            deString = deString.substr(spaceIndex + 1);
        }
        return deToken;
    }
	
};