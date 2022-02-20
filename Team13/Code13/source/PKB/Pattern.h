#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <tuple>
#include <vector>

#include "../common/Types.h"

class Pattern {
protected:
	static std::unordered_map<VarIndex, std::vector<std::tuple<StmtIndex, std::string>>, VarIndex::HashFunction> varPostFixTable;
	static std::unordered_map<std::string, std::vector<std::tuple<StmtIndex, VarIndex>>> postFixVarTable;

public:
	static void insertPostFixInfo(VarIndex varIdx, std::string postFixExpression, StmtIndex stmtIdx);
	static std::vector<int> getStmtsFromVarPattern(VarIndex varIdx, std::string expression, bool isSubExpression);
	static std::tuple<std::vector<int>, std::vector<int>> getStmtsFromPattern(std::string expression, bool isSubExpression);
	static std::vector<int> getStmtsFromVarPattern(VarIndex varIdx);
	static std::tuple<std::vector<int>, std::vector<int>> getAllAssignStmtVarsPatternInfo();
	static void performCleanUp();
};
