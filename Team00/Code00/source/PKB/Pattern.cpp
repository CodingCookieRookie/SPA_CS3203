#include "./Pattern.h"

using namespace std;

unordered_map<VarIndex, vector<tuple<StmtIndex, string>>, VarIndex::HashFunction> Pattern::varPostFixTable;
unordered_map<string, vector<tuple<StmtIndex, VarIndex>>> Pattern::postFixVarTable;

int Pattern::getVarPostFixTableSize() {
	return varPostFixTable.size();
}

int Pattern::getPostFixVarTableSize() {
	return postFixVarTable.size();
}

string Pattern::generatePostFixExpression(string infixExpression) {
	//TO DO
	string res;
	return res;
}

void Pattern::insertPostFixInfo(VarIndex varIdx, string postFixExpression, StmtIndex stmtIdx) {
	tuple<StmtIndex, string> varPostFixTuple = make_tuple(stmtIdx, postFixExpression);
	tuple<StmtIndex, VarIndex> postFixVarTuple = make_tuple(stmtIdx, varIdx);
	if (varPostFixTable.find(varIdx) == varPostFixTable.end()) {
		vector<tuple<StmtIndex, string>> value;
		value.push_back(varPostFixTuple);
		varPostFixTable[varIdx] = value;
	}
	else {
		varPostFixTable[varIdx].push_back(varPostFixTuple);
	}

	if (postFixVarTable.find(postFixExpression) == postFixVarTable.end()) {
		vector<tuple<StmtIndex, VarIndex>> value;
		value.push_back(postFixVarTuple);
		postFixVarTable[postFixExpression] = value;
	}
	else {
		postFixVarTable[postFixExpression].push_back(postFixVarTuple);
	}
}

vector<StmtIndex> Pattern::getStmtsFromVarPattern(VarIndex varIdx, string expression, bool isSubExpression) {
	vector<StmtIndex> res;

	vector<tuple<StmtIndex, string>> value = varPostFixTable[varIdx];
	for (auto& varPostFixTuple : value) {
		if (isSubExpression) {
			string storedExpression = get<1>(varPostFixTuple);
			if (storedExpression.find(expression) != std::string::npos) {
				res.push_back(get<0>(varPostFixTuple));
			}
		}
		else {
			res.push_back(get<0>(varPostFixTuple));
		}
	}

	return res;
}

vector<tuple<StmtIndex, VarIndex>> Pattern::getStmtsFromPattern(string expression, bool isSubExpression) {
	vector<tuple<StmtIndex, VarIndex>> res;

	if (isSubExpression) {
		for (auto& postFixVarInfo : postFixVarTable) {
			string postFixExpression = postFixVarInfo.first;
			if (postFixExpression.find(expression) != std::string::npos) {
				vector<tuple<StmtIndex, VarIndex>> value = postFixVarTable[postFixExpression];
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

vector<StmtIndex> Pattern::getStmtsFromVarPattern(VarIndex varIdx) {
	vector<StmtIndex> res;

	vector<tuple<StmtIndex, string>> value = varPostFixTable[varIdx];
	for (auto& varPostFixTuple : value) {
		res.push_back(get<0>(varPostFixTuple));
	}

	return res;
}

vector<tuple<StmtIndex, VarIndex>> Pattern::getAllAssignStmtVarsPatternInfo() {
	vector<tuple<StmtIndex, VarIndex>> res;

	for (auto& varPostFixInfo : varPostFixTable) {
		VarIndex varIdx = varPostFixInfo.first;
		vector<tuple<StmtIndex, string>> value = varPostFixTable[varIdx];
		for (auto& varPostFixTuple : value) {
			res.push_back(make_tuple(get<0>(varPostFixTuple), varIdx));
		}
	}

	return res;
}

void Pattern::performCleanUp() {
	varPostFixTable = {};
	postFixVarTable = {};
}
