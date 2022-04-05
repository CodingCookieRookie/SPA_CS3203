#pragma once

#include <string>
#include <tuple>
#include <vector>

#include "../Common/Types.h"
#include "./BidirectionalTable/BidirectionalTableOneWaySet.h"

class PatternAssign {
private:
	BidirectionalTableOneWaySet<VarIndex, StmtIndex> assignedVarStmtBidirectionalTable;
	BidirectionalTableOneWaySet<std::string, StmtIndex> postfixExprStmtBidirectionalTable;

public:
	PatternAssign();
	void insertAssignInfo(VarIndex& varIdx, std::string& postFixExpression, StmtIndex& stmtIdx);
	std::vector<StmtIndex> getAssignStmtsFromVarExprFullMatch(VarIndex varIdx, std::string& expression);
	std::vector<StmtIndex> getAssignStmtsFromVarExprPartialMatch(VarIndex varIdx, std::string& expression);
	std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> getAssignStmtsFromExprFullMatch(std::string& expression);
	std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> getAssignStmtsFromExprPartialMatch(std::string& expression);
	std::vector<StmtIndex> getAssignStmtsFromVar(VarIndex& varIdx);
	std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> getAllAssignPatternInfo();
};
