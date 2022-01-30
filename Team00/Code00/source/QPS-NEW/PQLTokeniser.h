#pragma once
#include <vector>
#include "PQLDEToken.h"

void tokeniseQuery(vector<string> query);
vector<PQLDEToken> getPQLDETokens();
vector<string> getSynonyms();

