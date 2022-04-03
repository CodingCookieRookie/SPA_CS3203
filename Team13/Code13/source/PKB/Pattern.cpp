#include "./Pattern.h"

BidirectionalTableOneWaySet<VarIndex, StmtIndex> Pattern::assignedVarStmtBidirectionalTable;
BidirectionalTableOneWaySet<std::string, StmtIndex> Pattern::postfixExprStmtBidirectionalTable;

std::unordered_map<VarIndex, std::unordered_set<StmtIndex>> Pattern::ifVarTable;
std::unordered_map<VarIndex, std::unordered_set<StmtIndex>> Pattern::whileVarTable;

void Pattern::insertAssignInfo(VarIndex& varIdx, std::string& postFixExpression, StmtIndex& stmtIdx) {
	assignedVarStmtBidirectionalTable.insert(varIdx, stmtIdx);
	postfixExprStmtBidirectionalTable.insert(postFixExpression, stmtIdx);
}

void Pattern::insertIfInfo(StmtIndex& stmtIdx, VarIndex& varIdx) {
	ifVarTable[varIdx].insert(stmtIdx);
}

void Pattern::insertWhileInfo(StmtIndex& stmtIdx, VarIndex& varIdx) {
	whileVarTable[varIdx].insert(stmtIdx);
}

std::vector<StmtIndex> Pattern::getAssignStmtsFromVarExprFullMatch(VarIndex varIdx, std::string& expression) {
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

std::vector<StmtIndex> Pattern::getAssignStmtsFromVarExprPartialMatch(VarIndex varIdx, std::string& expression) {
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

std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> Pattern::getAssignStmtsFromExprFullMatch(std::string& expression) {
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

std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> Pattern::getAssignStmtsFromExprPartialMatch(std::string& expression) {
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

std::vector<StmtIndex> Pattern::getAssignStmtsFromVar(VarIndex& varIdx) {
	return assignedVarStmtBidirectionalTable.getFromLeftArg(varIdx);
}

std::vector<StmtIndex> Pattern::getIfStmtsFromVar(VarIndex& varIndex) {
	std::vector<StmtIndex> res;

	for (auto& stmtIdx : ifVarTable[varIndex]) {
		res.push_back(stmtIdx);
	}

	return res;
}

std::vector<StmtIndex> Pattern::getWhileStmtsFromVar(VarIndex& varIndex) {
	std::vector<StmtIndex> res;

	for (auto& stmtIdx : whileVarTable[varIndex]) {
		res.push_back(stmtIdx);
	}

	return res;
}

std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> Pattern::getAllAssignPatternInfo() {
	return assignedVarStmtBidirectionalTable.getAll();
}

std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> Pattern::getAllIfPatternInfo() {
	std::vector<StmtIndex> stmtIndices;
	std::vector<VarIndex> varIndices;

	for (auto& ifVarInfo : ifVarTable) {
		VarIndex varIdx = ifVarInfo.first;
		for (auto& stmtIdx : ifVarInfo.second) {
			stmtIndices.push_back(stmtIdx);
			varIndices.push_back(varIdx);
		}
	}

	return std::make_tuple(varIndices, stmtIndices);
}
std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> Pattern::getAllWhilePatternInfo() {
	std::vector<StmtIndex> stmtIndices;
	std::vector<VarIndex> varIndices;

	for (auto& whileVarInfo : whileVarTable) {
		VarIndex varIdx = whileVarInfo.first;
		for (auto& stmtIdx : whileVarInfo.second) {
			stmtIndices.push_back(stmtIdx);
			varIndices.push_back(varIdx);
		}
	}

	return std::make_tuple(varIndices, stmtIndices);
}

void Pattern::performCleanUp() {
	assignedVarStmtBidirectionalTable = BidirectionalTableOneWaySet<VarIndex, StmtIndex>();
	postfixExprStmtBidirectionalTable = BidirectionalTableOneWaySet<std::string, StmtIndex>();
	ifVarTable = {};
	whileVarTable = {};
}
