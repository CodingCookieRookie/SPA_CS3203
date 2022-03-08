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
	static std::unordered_map<VarIndex, std::unordered_set<StmtIndex>> ifVarTable;
	static std::unordered_map<VarIndex, std::unordered_set<StmtIndex>> whileVarTable;

public:
	static void insertAssignInfo(VarIndex& varIdx, std::string& postFixExpression, StmtIndex& stmtIdx);
	static void insertIfInfo(StmtIndex& stmtIdx, VarIndex& varIdx);
	static void insertWhileInfo(StmtIndex& stmtIdx, VarIndex& varIdx);
	static std::vector<int> getAssignStmtsFromVarExprFullMatch(VarIndex& varIdx, std::string& expression);
	static std::vector<int> getAssignStmtsFromVarExprPartialMatch(VarIndex& varIdx, std::string& expression);
	static std::tuple<std::vector<int>, std::vector<int>> getAssignStmtsFromExprFullMatch(std::string& expression);
	static std::tuple<std::vector<int>, std::vector<int>> getAssignStmtsFromExprPartialMatch(std::string& expression);
	static std::vector<int> getAssignStmtsFromVar(VarIndex& varIdx);
	static std::vector<int> getIfStmtsFromVar(VarIndex& varIndex);
	static std::vector<int> getWhileStmtsFromVar(VarIndex& varIndex);
	static std::tuple<std::vector<int>, std::vector<int>> getAllAssignPatternInfo();
	static std::tuple<std::vector<int>, std::vector<int>> getAllIfPatternInfo();
	static std::tuple<std::vector<int>, std::vector<int>> getAllWhilePatternInfo();
	static void performCleanUp();
};
