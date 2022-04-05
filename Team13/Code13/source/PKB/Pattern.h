#pragma once

#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../Common/Types.h"
#include "./BidirectionalTable/BidirectionalTableOneWaySet.h"

class Pattern {
private:
	static BidirectionalTableOneWaySet<VarIndex, StmtIndex> assignedVarStmtBidirectionalTable;
	static BidirectionalTableOneWaySet<std::string, StmtIndex> postfixExprStmtBidirectionalTable;
	static std::unordered_map<VarIndex, std::unordered_set<StmtIndex>> ifVarTable;
	static std::unordered_map<VarIndex, std::unordered_set<StmtIndex>> whileVarTable;

public:
	static void insertAssignInfo(VarIndex& varIdx, std::string& postFixExpression, StmtIndex& stmtIdx);
	static void insertIfInfo(StmtIndex& stmtIdx, VarIndex& varIdx);
	static void insertWhileInfo(StmtIndex& stmtIdx, VarIndex& varIdx);
	static std::vector<StmtIndex> getAssignStmtsFromVarExprFullMatch(VarIndex varIdx, std::string& expression);
	static std::vector<StmtIndex> getAssignStmtsFromVarExprPartialMatch(VarIndex varIdx, std::string& expression);
	static std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> getAssignStmtsFromExprFullMatch(std::string& expression);
	static std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> getAssignStmtsFromExprPartialMatch(std::string& expression);
	static std::vector<StmtIndex> getAssignStmtsFromVar(VarIndex& varIdx);
	static std::vector<StmtIndex> getIfStmtsFromVar(VarIndex& varIndex);
	static std::vector<StmtIndex> getWhileStmtsFromVar(VarIndex& varIndex);
	static std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> getAllAssignPatternInfo();
	static std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> getAllIfPatternInfo();
	static std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> getAllWhilePatternInfo();
	static void performCleanUp();
};
