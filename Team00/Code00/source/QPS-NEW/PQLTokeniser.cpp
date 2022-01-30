
using namespace std;

#include "PQLTokeniser.h"

const string Select = "Select";
const int SelectLength = 6;
vector<PQLDEToken> PQLDETokens;
vector<string> synonyms;


void tokeniseQuery(vector<string> userQuery)
{
    // Combine all queries
    // Take the string from start until see the word 'Select' and Handle those as PQLDETokens
    // Take the rest off the string and Handle the rest as ClausesToken
    string wholeQuery;
    // should changge this to incorporate both PQLDEToken and Clauses token
    for (int i = 0; i < userQuery.size(); i++) {
        wholeQuery += userQuery.at(i);
    }
    size_t select = wholeQuery.find(Select);
    if (select == string::npos) {
        return;
    } else {
        // Select is found and statement is valid for now
        string entireDeString = wholeQuery.substr(0, select);
        cout << "check deString: " << entireDeString << "\n";
        size_t index = entireDeString.find(";");
        while (index != string::npos) {
            size_t firstNonSpace = entireDeString.find_first_not_of(" ");   // to eliminate any trailing spaces at start of deString
            string deString = entireDeString.substr(firstNonSpace, index);
            PQLDETokens.push_back(PQLDEToken::getPQLDEToken(deString));
            entireDeString = entireDeString.substr(index + 1);
            index = entireDeString.find(";");
        }
        //vector<string> synonyms => //s
        //vector<PQLDEToken> //s
        //iterate thru synonyms -> //s
        //   iterate thru vector<PQLDEClauses> - > //null
        //  
        //return all rs w return type s (stmt)
    }
    // to be worked on 
    //string afterSelect = wholeQuery.substr(select + SelectLength);
    //int clauseIndex = afterSelect.find("such");
    //string synonymsString = afterSelect.substr(0, clauseIndex);
    //std::string delimiter = ",";
    //std::string token = s.substr(0, s.find(delimiter));
    //s.erase(0, s.find(delimiter) + delimiter.length());
}

vector<PQLDEToken> getPQLDETokens() {
    return PQLDETokens;
}

vector<string> getSynonyms() {
    return synonyms;
}
