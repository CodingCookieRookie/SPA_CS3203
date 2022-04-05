#include "./PatternAssign.h"

PatternAssign::PatternAssign() {}

void PatternAssign::insertAssignInfo(VarIndex& varIdx, std::string& postFixExpression, StmtIndex& stmtIdx) {
	assignedVarStmtBidirectionalTable.insert(varIdx, stmtIdx);
	postfixExprStmtBidirectionalTable.insert(postFixExpression, stmtIdx);
}

std::vector<StmtIndex> PatternAssign::getAssignStmtsFromVarExprFullMatch(VarIndex varIdx, std::string& expression) {
	std::vector<StmtIndex> res;

	std::vector<StmtIndex> stmtsFromVar = assignedVarStmtBidirectionalTable.getFromLeftArg(varIdx);
	std::vector<StmtIndex> stmtsFromExpr = postfixExprStmtBidirectionalTable.getFromLeftArg(expression);

	for (auto& stmtIdxFromVar : stmtsFromVar) {
		for (auto& stmtIdxFromExpr : stmtsFromExpr) {
			if (stmtIdxFromVar == stmtIdxFromExpr) {
				res.push_back(stmtIdxFromVar);
			}
		}
	}

	return res;
}

std::vector<StmtIndex> PatternAssign::getAssignStmtsFromVarExprPartialMatch(VarIndex varIdx, std::string& expression) {
	std::vector<StmtIndex> res;

	std::vector<StmtIndex> stmtsFromVar = assignedVarStmtBidirectionalTable.getFromLeftArg(varIdx);

	for (auto& stmtIdxFromVar : stmtsFromVar) {
		std::string storedExpression = postfixExprStmtBidirectionalTable.getFromRightArg(stmtIdxFromVar);
		if (storedExpression.find(expression) != std::string::npos) {
			res.push_back(stmtIdxFromVar);
		}
	}

	return res;
}

std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> PatternAssign::getAssignStmtsFromExprFullMatch(std::string& expression) {
	std::vector<StmtIndex> stmtIndices;
	std::vector<VarIndex> varIndices;

	std::vector<StmtIndex> stmtsFromExpr = postfixExprStmtBidirectionalTable.getFromLeftArg(expression);

	for (auto& stmtIdxFromExpr : stmtsFromExpr) {
		VarIndex varIdx = assignedVarStmtBidirectionalTable.getFromRightArg(stmtIdxFromExpr);
		stmtIndices.push_back(stmtIdxFromExpr);
		varIndices.push_back(varIdx);
	}

	return std::make_tuple(varIndices, stmtIndices);
}

std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> PatternAssign::getAssignStmtsFromExprPartialMatch(std::string& expression) {
	std::vector<StmtIndex> stmtIndices;
	std::vector<VarIndex> varIndices;

	std::tuple<std::vector<std::string>, std::vector<StmtIndex>> postFixExprStmtInfo = postfixExprStmtBidirectionalTable.getAll();

	for (size_t vectorIdx = 0; vectorIdx < std::get<0>(postFixExprStmtInfo).size(); vectorIdx++) {
		std::string storedExpression = std::get<0>(postFixExprStmtInfo).at(vectorIdx);
		if (storedExpression.find(expression) == std::string::npos) {
			continue;
		}

		StmtIndex stmtIdx = std::get<1>(postFixExprStmtInfo).at(vectorIdx);
		VarIndex varIdx = assignedVarStmtBidirectionalTable.getFromRightArg(stmtIdx);
		stmtIndices.push_back(stmtIdx);
		varIndices.push_back(varIdx);
	}

	return std::make_tuple(varIndices, stmtIndices);
}

std::vector<StmtIndex> PatternAssign::getAssignStmtsFromVar(VarIndex& varIdx) {
	return assignedVarStmtBidirectionalTable.getFromLeftArg(varIdx);
}

std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> PatternAssign::getAllAssignPatternInfo() {
	return assignedVarStmtBidirectionalTable.getAll();
}
