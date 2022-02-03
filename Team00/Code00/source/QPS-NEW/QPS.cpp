#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "PQLTokeniser.h"

using namespace std;


string processQuery (vector<string> query) {	// This should be an array of size 2 because there can be a max of 2 lines?
	tokeniseQuery(query);
	vector<PQLDEToken> pqldeTokens = getPQLDETokens();
	string res;
	for (size_t i = 0; i < query.size(); i++) {
		res = res + pqldeTokens.at(i).getPQLDETokenString() + "\n";
	}
	return res;
}






