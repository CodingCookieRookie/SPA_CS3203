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
	static std::vector<StmtIndex> getAssignStmtsFromVarExprFullMatch(VarIndex varIdx, std::string& expression);
	static std::vector<StmtIndex> getAssignStmtsFromVarExprPartialMatch(VarIndex varIdx, std::string& expression);
	static std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> getAssignStmtsFromExprFullMatch(std::string& expression);
	static std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> getAssignStmtsFromExprPartialMatch(std::string& expression);
	static std::vector<StmtIndex> getAssignStmtsFromVar(VarIndex& varIdx);
	static std::vector<StmtIndex> getIfStmtsFromVar(VarIndex& varIndex);
	static std::vector<StmtIndex> getWhileStmtsFromVar(VarIndex& varIndex);
	static std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> getAllAssignPatternInfo();
	static std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> getAllIfPatternInfo();
	static std::tuple<std::vector<StmtIndex>, std::vector<VarIndex>> getAllWhilePatternInfo();
	static void performCleanUp();
};
