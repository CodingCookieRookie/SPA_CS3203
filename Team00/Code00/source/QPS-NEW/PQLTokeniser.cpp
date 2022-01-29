
using namespace std;

#include "PQLTokeniser.h"

const string Select = "Select";

vector<PQLDEToken> tokeniseQuery(vector<string> userQuery)
{
    // Combine all queries
    // Take the string from start until see the word 'Select' and Handle those as PQLDETokens
    // Take the rest off the string and Handle the rest as ClausesToken
    string wholeQuery;
    vector<PQLDEToken> PQLDETokens; // should changge this to incorporate both PQLDEToken and Clauses token
    for (int i = 0; i < userQuery.size(); i++) {
        wholeQuery += userQuery.at(i);
    }
    size_t select = wholeQuery.find(Select);
    if (select == string::npos) {
        return PQLDETokens;
    } else {
        // Select is found and statement is valid for now
        string deString = wholeQuery.substr(0, select);
        cout << "check deString: " << deString;
        PQLDETokens.push_back(PQLDEToken::getPQLDEToken(deString));
    }
    return PQLDETokens;
}
