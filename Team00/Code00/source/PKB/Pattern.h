#pragma once

#ifndef PATTERN_H
#define PATTERN_H

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <tuple>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <tuple>
#include <vector>

#include "../common/Types.h"

class Pattern {
protected:
	static int getVarPostFixTableSize();
	static int getPostFixVarTableSize();
	static unordered_map<VarIndex, vector<tuple<StmtIndex, string>>, VarIndex::HashFunction> varPostFixTable;
	static unordered_map<string, vector<tuple<StmtIndex, VarIndex>>> postFixVarTable;

public:
	static string generatePostFixExpression(string infixExpression);
	static void insertPostFixInfo(VarIndex varIdx, string postFixExpression, StmtIndex stmtIdx);
	static vector<StmtIndex> getStmtsFromVarPattern(VarIndex varIdx, string expression, bool isSubExpression);
	static vector<tuple<StmtIndex, VarIndex>> getStmtsFromPattern(string expression, bool isSubExpression);
	static vector<StmtIndex> getStmtsFromVarPattern(VarIndex varIdx);
	static vector<tuple<StmtIndex, VarIndex>> getAllAssignStmtVarsPatternInfo();
	static void performCleanUp();
};

#endif
