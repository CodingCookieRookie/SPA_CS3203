#include "./Pattern.h"

std::unordered_map<VarIndex, std::vector<std::tuple<StmtIndex, std::string>>> Pattern::varPostFixTable;
std::unordered_map<std::string, std::vector<std::tuple<StmtIndex, VarIndex>>> Pattern::postFixVarTable;
std::unordered_map<VarIndex, std::unordered_set<StmtIndex>> Pattern::ifVarTable;
std::unordered_map<VarIndex, std::unordered_set<StmtIndex>> Pattern::whileVarTable;

void Pattern::insertAssignInfo(VarIndex& varIdx, std::string& postFixExpression, StmtIndex& stmtIdx) {
	std::tuple<StmtIndex, std::string> varPostFixTuple = std::make_tuple(stmtIdx, postFixExpression);
	std::tuple<StmtIndex, VarIndex> postFixVarTuple = std::make_tuple(stmtIdx, varIdx);
	if (varPostFixTable.find(varIdx) == varPostFixTable.end()) {
		std::vector<std::tuple<StmtIndex, std::string>> value;
		value.push_back(varPostFixTuple);
		varPostFixTable[varIdx] = value;
	} else {
		varPostFixTable[varIdx].push_back(varPostFixTuple);
	}

	if (postFixVarTable.find(postFixExpression) == postFixVarTable.end()) {
		std::vector<std::tuple<StmtIndex, VarIndex>> value;
		value.push_back(postFixVarTuple);
		postFixVarTable[postFixExpression] = value;
	} else {
		postFixVarTable[postFixExpression].push_back(postFixVarTuple);
	}
}

void Pattern::insertIfInfo(StmtIndex& stmtIdx, VarIndex& varIdx) {
	ifVarTable[varIdx].insert(stmtIdx);
}

void Pattern::insertWhileInfo(StmtIndex& stmtIdx, VarIndex& varIdx) {
	whileVarTable[varIdx].insert(stmtIdx);
}

std::vector<int> Pattern::getAssignStmtsFromVarExprFullMatch(VarIndex varIdx, std::string& expression) {
	std::vector<int> res;
	std::unordered_set<StmtIndex> stmtSet;

	std::vector<std::tuple<StmtIndex, std::string>> value = varPostFixTable[varIdx];
	for (auto& varPostFixTuple : value) {
		std::string storedExpression = std::get<1>(varPostFixTuple);
		if (storedExpression == expression) {
			StmtIndex stmtIdx = std::get<0>(varPostFixTuple);
			stmtSet.insert(stmtIdx);
		}
	}

	for (auto& stmtIdx : stmtSet) {
		res.push_back(stmtIdx);
	}

	return res;
}

std::vector<int> Pattern::getAssignStmtsFromVarExprPartialMatch(VarIndex varIdx, std::string& expression) {
	std::vector<int> res;
	std::unordered_set<StmtIndex> stmtSet;

	std::vector<std::tuple<StmtIndex, std::string>> value = varPostFixTable[varIdx];
	for (auto& varPostFixTuple : value) {
		std::string storedExpression = std::get<1>(varPostFixTuple);
		if (storedExpression.find(expression) != std::string::npos) {
			StmtIndex stmtIdx = std::get<0>(varPostFixTuple);
			stmtSet.insert(stmtIdx);
		}
	}

	for (auto& stmtIdx : stmtSet) {
		res.push_back(stmtIdx);
	}

	return res;
}

std::tuple<std::vector<int>, std::vector<int>> Pattern::getAssignStmtsFromExprFullMatch(std::string& expression) {
	std::vector<int> stmtIndices;
	std::vector<int> varIndices;

	std::vector<std::tuple<StmtIndex, VarIndex>> value = postFixVarTable[expression];
	for (auto& postFixVarTuple : value) {
		stmtIndices.push_back(std::get<0>(postFixVarTuple));
		varIndices.push_back(std::get<1>(postFixVarTuple));
	}

	return std::make_tuple(stmtIndices, varIndices);
}

std::tuple<std::vector<int>, std::vector<int>> Pattern::getAssignStmtsFromExprPartialMatch(std::string& expression) {
	std::vector<int> stmtIndices;
	std::vector<int> varIndices;

	for (auto& postFixVarInfo : postFixVarTable) {
		std::string postFixExpression = postFixVarInfo.first;
		if (postFixExpression.find(expression) == std::string::npos) {
			continue;
		}
		std::vector<std::tuple<StmtIndex, VarIndex>> value = postFixVarTable[postFixExpression];
		for (auto& postFixVarTuple : value) {
			stmtIndices.push_back(std::get<0>(postFixVarTuple));
			varIndices.push_back(std::get<1>(postFixVarTuple));
		}
	}

	return std::make_tuple(stmtIndices, varIndices);
}

std::vector<int> Pattern::getAssignStmtsFromVar(VarIndex& varIdx) {
	std::vector<int> res;
	std::unordered_set<StmtIndex> stmtSet;

	std::vector<std::tuple<StmtIndex, std::string>> value = varPostFixTable[varIdx];
	for (auto& varPostFixTuple : value) {
		stmtSet.insert(std::get<0>(varPostFixTuple));
	}

	for (auto& stmtIdx : stmtSet) {
		res.push_back(stmtIdx);
	}

	return res;
}

std::vector<int> Pattern::getIfStmtsFromVar(VarIndex& varIndex) {
	std::vector<int> res;

	for (auto& stmtIdx : ifVarTable[varIndex]) {
		res.push_back(stmtIdx);
	}

	return res;
}

std::vector<int> Pattern::getWhileStmtsFromVar(VarIndex& varIndex) {
	std::vector<int> res;

	for (auto& stmtIdx : whileVarTable[varIndex]) {
		res.push_back(stmtIdx);
	}

	return res;
}

std::tuple<std::vector<int>, std::vector<int>> Pattern::getAllAssignPatternInfo() {
	std::vector<int> stmtIndices;
	std::vector<int> varIndices;

	for (auto& varPostFixInfo : varPostFixTable) {
		VarIndex varIdx = varPostFixInfo.first;
		std::vector<std::tuple<StmtIndex, std::string>> value = varPostFixTable[varIdx];
		for (auto& varPostFixTuple : value) {
			stmtIndices.push_back(std::get<0>(varPostFixTuple));
			varIndices.push_back(varIdx);
		}
	}

	return std::make_tuple(stmtIndices, varIndices);
}

std::tuple<std::vector<int>, std::vector<int>> Pattern::getAllIfPatternInfo() {
	std::vector<int> stmtIndices;
	std::vector<int> varIndices;

	for (auto& ifVarInfo : ifVarTable) {
		VarIndex varIdx = ifVarInfo.first;
		for (auto& stmtIdx : ifVarInfo.second) {
			stmtIndices.push_back(stmtIdx);
			varIndices.push_back(varIdx);
		}
	}

	return std::make_tuple(stmtIndices, varIndices);
}
std::tuple<std::vector<int>, std::vector<int>> Pattern::getAllWhilePatternInfo() {
	std::vector<int> stmtIndices;
	std::vector<int> varIndices;

	for (auto& whileVarInfo : whileVarTable) {
		VarIndex varIdx = whileVarInfo.first;
		for (auto& stmtIdx : whileVarInfo.second) {
			stmtIndices.push_back(stmtIdx);
			varIndices.push_back(varIdx);
		}
	}

	return std::make_tuple(stmtIndices, varIndices);
}

void Pattern::performCleanUp() {
	varPostFixTable = {};
	postFixVarTable = {};
	ifVarTable = {};
	whileVarTable = {};
}
