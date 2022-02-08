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
	static std::unordered_map<VarIndex, std::vector<std::tuple<StmtIndex, std::string>>, VarIndex::HashFunction> varPostFixTable;
	static std::unordered_map<std::string, std::vector<std::tuple<StmtIndex, VarIndex>>> postFixVarTable;

public:
	static std::string generatePostFixExpression(std::string infixExpression);
	static void insertPostFixInfo(VarIndex varIdx, std::string postFixExpression, StmtIndex stmtIdx);
	static std::vector<StmtIndex> getStmtsFromVarPattern(VarIndex varIdx, std::string expression, bool isSubExpression);
	static std::vector<std::tuple<StmtIndex, VarIndex>> getStmtsFromPattern(std::string expression, bool isSubExpression);
	static std::vector<StmtIndex> getStmtsFromVarPattern(VarIndex varIdx);
	static std::vector<std::tuple<StmtIndex, VarIndex>> getAllAssignStmtVarsPatternInfo();
	static void performCleanUp();
};

#endif
