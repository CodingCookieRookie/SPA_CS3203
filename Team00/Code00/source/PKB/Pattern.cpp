#include "./Pattern.h"

std::unordered_map<VarIndex, std::vector<std::tuple<StmtIndex, std::string>>, VarIndex::HashFunction> Pattern::varPostFixTable;
std::unordered_map<std::string, std::vector<std::tuple<StmtIndex, VarIndex>>> Pattern::postFixVarTable;

// TO MOVE TO ENTITIES
std::string Pattern::generatePostFixExpression(std::string infixExpression) {
	//TO DO
	std::string res;
	return res;
}

void Pattern::insertPostFixInfo(VarIndex varIdx, std::string postFixExpression, StmtIndex stmtIdx) {
	std::tuple<StmtIndex, std::string> varPostFixTuple = std::make_tuple(stmtIdx, postFixExpression);
	std::tuple<StmtIndex, VarIndex> postFixVarTuple = std::make_tuple(stmtIdx, varIdx);
	if (varPostFixTable.find(varIdx) == varPostFixTable.end()) {
		std::vector<std::tuple<StmtIndex, std::string>> value;
		value.push_back(varPostFixTuple);
		varPostFixTable[varIdx] = value;
	}
	else {
		varPostFixTable[varIdx].push_back(varPostFixTuple);
	}

	if (postFixVarTable.find(postFixExpression) == postFixVarTable.end()) {
		std::vector<std::tuple<StmtIndex, VarIndex>> value;
		value.push_back(postFixVarTuple);
		postFixVarTable[postFixExpression] = value;
	}
	else {
		postFixVarTable[postFixExpression].push_back(postFixVarTuple);
	}
}

std::vector<StmtIndex> Pattern::getStmtsFromVarPattern(VarIndex varIdx, std::string expression, bool isSubExpression) {
	std::vector<StmtIndex> res;
	std::unordered_set<StmtIndex, StmtIndex::HashFunction> stmtSet;

	std::vector<std::tuple<StmtIndex, std::string>> value = varPostFixTable[varIdx];
	for (auto& varPostFixTuple : value) {
		std::string storedExpression = std::get<1>(varPostFixTuple);
		if (isSubExpression) {
			if (storedExpression.find(expression) != std::string::npos) {
				StmtIndex stmtIdx = std::get<0>(varPostFixTuple);
				stmtSet.insert(stmtIdx);
			}
		}
		else {
			if (storedExpression == expression) {
				StmtIndex stmtIdx = std::get<0>(varPostFixTuple);
				stmtSet.insert(stmtIdx);
			}
		}
	}

	for (auto& stmtIdx : stmtSet) {
		res.push_back(stmtIdx);
	}

	return res;
}

std::vector<std::tuple<StmtIndex, VarIndex>> Pattern::getStmtsFromPattern(std::string expression, bool isSubExpression) {
	std::vector<std::tuple<StmtIndex, VarIndex>> res;

	if (isSubExpression) {
		for (auto& postFixVarInfo : postFixVarTable) {
			std::string postFixExpression = postFixVarInfo.first;
			if (postFixExpression.find(expression) != std::string::npos) {
				std::vector<std::tuple<StmtIndex, VarIndex>> value = postFixVarTable[postFixExpression];
				for (auto& postFixVarTuple : value) {
					res.push_back(postFixVarTuple);
				}
			}
		}
	}
	else {
		res = postFixVarTable[expression];
	}

	return res;
}

std::vector<StmtIndex> Pattern::getStmtsFromVarPattern(VarIndex varIdx) {
	std::vector<StmtIndex> res;
	std::unordered_set<StmtIndex, StmtIndex::HashFunction> stmtSet;

	std::vector<std::tuple<StmtIndex, std::string>> value = varPostFixTable[varIdx];
	for (auto& varPostFixTuple : value) {
		stmtSet.insert(std::get<0>(varPostFixTuple));
	}

	for (auto& stmtIdx : stmtSet) {
		res.push_back(stmtIdx);
	}

	return res;
}

std::vector<std::tuple<StmtIndex, VarIndex>> Pattern::getAllAssignStmtVarsPatternInfo() {
	std::vector<std::tuple<StmtIndex, VarIndex>> res;

	for (auto& varPostFixInfo : varPostFixTable) {
		VarIndex varIdx = varPostFixInfo.first;
		std::vector<std::tuple<StmtIndex, std::string>> value = varPostFixTable[varIdx];
		for (auto& varPostFixTuple : value) {
			res.push_back(std::make_tuple(std::get<0>(varPostFixTuple), varIdx));
		}
	}

	return res;
}

void Pattern::performCleanUp() {
	varPostFixTable = {};
	postFixVarTable = {};
}
