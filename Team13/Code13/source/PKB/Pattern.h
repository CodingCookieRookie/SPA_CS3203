#pragma once

#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../Common/Types.h"

class Pattern {
protected:
	static std::unordered_map<VarIndex, std::vector<std::tuple<StmtIndex, std::string>>> varPostFixTable;
	static std::unordered_map<std::string, std::vector<std::tuple<StmtIndex, VarIndex>>> postFixVarTable;

public:
	static void insertPostFixInfo(VarIndex& varIdx, std::string& postFixExpression, StmtIndex& stmtIdx);
	static std::vector<int> getStmtsFromVarPatternFullMatch(VarIndex& varIdx, std::string& postFixExpression);
	static std::vector<int> getStmtsFromVarPatternPartialMatch(VarIndex& varIdx, std::string& postFixExpression);
	static std::tuple<std::vector<int>, std::vector<int>> getStmtsFromPatternFullMatch(std::string& expression);
	static std::tuple<std::vector<int>, std::vector<int>> getStmtsFromPatternPartialMatch(std::string& expression);
	static std::vector<int> getStmtsFromVarPattern(VarIndex& varIdx);
	static std::tuple<std::vector<int>, std::vector<int>> getAllAssignStmtVarsPatternInfo();
	static void performCleanUp();
};
