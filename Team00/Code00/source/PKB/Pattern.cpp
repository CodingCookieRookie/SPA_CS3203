#include "./Pattern.h"

std::unordered_map<VarIndex, std::vector<std::tuple<StmtIndex, std::string>>, VarIndex::HashFunction> Pattern::varPostFixTable;
std::unordered_map<std::string, std::vector<std::tuple<StmtIndex, VarIndex>>> Pattern::postFixVarTable;

void Pattern::insertPostFixInfo(VarIndex varIdx, std::string postFixExpression, StmtIndex stmtIdx) {
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

std::vector<int> Pattern::getStmtsFromVarPattern(VarIndex varIdx, std::string expression, bool isSubExpression) {
	std::vector<int> res;
	std::unordered_set<StmtIndex, StmtIndex::HashFunction> stmtSet;

	std::vector<std::tuple<StmtIndex, std::string>> value = varPostFixTable[varIdx];
	for (auto& varPostFixTuple : value) {
		std::string storedExpression = std::get<1>(varPostFixTuple);
		if (isSubExpression) {
			if (storedExpression.find(expression) != std::string::npos) {
				StmtIndex stmtIdx = std::get<0>(varPostFixTuple);
				stmtSet.insert(stmtIdx);
			}
		} else {
			if (storedExpression == expression) {
				StmtIndex stmtIdx = std::get<0>(varPostFixTuple);
				stmtSet.insert(stmtIdx);
			}
		}
	}

	for (auto& stmtIdx : stmtSet) {
		res.push_back(stmtIdx.index);
	}

	return res;
}

std::tuple<std::vector<int>, std::vector<int>> Pattern::getStmtsFromPattern(std::string expression, bool isSubExpression) {
	std::vector<int> stmtIndices;
	std::vector<int> varIndices;

	if (!isSubExpression) {
		std::vector<std::tuple<StmtIndex, VarIndex>> value = postFixVarTable[expression];
		for (auto& postFixVarTuple : value) {
			stmtIndices.push_back(std::get<0>(postFixVarTuple).index);
			varIndices.push_back(std::get<1>(postFixVarTuple).index);
		}
	} else {
		for (auto& postFixVarInfo : postFixVarTable) {
			std::string postFixExpression = postFixVarInfo.first;
			if (postFixExpression.find(expression) == std::string::npos) {
				continue;
			}
			std::vector<std::tuple<StmtIndex, VarIndex>> value = postFixVarTable[postFixExpression];
			for (auto& postFixVarTuple : value) {
				stmtIndices.push_back(std::get<0>(postFixVarTuple).index);
				varIndices.push_back(std::get<1>(postFixVarTuple).index);
			}
		}
	}

	return std::make_tuple(stmtIndices, varIndices);
}

std::vector<int> Pattern::getStmtsFromVarPattern(VarIndex varIdx) {
	std::vector<int> res;
	std::unordered_set<StmtIndex, StmtIndex::HashFunction> stmtSet;

	std::vector<std::tuple<StmtIndex, std::string>> value = varPostFixTable[varIdx];
	for (auto& varPostFixTuple : value) {
		stmtSet.insert(std::get<0>(varPostFixTuple));
	}

	for (auto& stmtIdx : stmtSet) {
		res.push_back(stmtIdx.index);
	}

	return res;
}

std::tuple<std::vector<int>, std::vector<int>> Pattern::getAllAssignStmtVarsPatternInfo() {
	std::vector<int> stmtIndices;
	std::vector<int> varIndices;

	for (auto& varPostFixInfo : varPostFixTable) {
		VarIndex varIdx = varPostFixInfo.first;
		std::vector<std::tuple<StmtIndex, std::string>> value = varPostFixTable[varIdx];
		for (auto& varPostFixTuple : value) {
			stmtIndices.push_back(std::get<0>(varPostFixTuple).index);
			varIndices.push_back(varIdx.index);
		}
	}

	return std::make_tuple(stmtIndices, varIndices);
}

void Pattern::performCleanUp() {
	varPostFixTable = {};
	postFixVarTable = {};
}
