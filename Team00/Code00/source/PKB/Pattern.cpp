#include "./Pattern.h"

using namespace std;

unordered_map<VarIndex, vector<tuple<StmtIndex, string>>, VarIndex::HashFunction> Pattern::varPostFixTable;
unordered_map<string, vector<tuple<StmtIndex, VarIndex>>> Pattern::postFixVarTable;

int Pattern::getVarPostFixTableSize() {
	return -1;
}

int Pattern::getPostFixVarTableSize() {
	return -1;
}

vector<StmtIndex> Pattern::getStmtsFromVarPattern(VarIndex varIdx, string expression, bool isSubExpression) {
	vector<StmtIndex> res;
	return res;
}

vector<tuple<StmtIndex, VarIndex>> Pattern::getStmtsFromPattern(string expression, bool isSubExpression) {
	vector<tuple<StmtIndex, VarIndex>> res;
	return res;
}

vector<StmtIndex> Pattern::getStmtsFromVarPattern(VarIndex varIdx) {
	vector<StmtIndex> res;
	return res;
}

vector<tuple<StmtIndex, VarIndex>> Pattern::getAllAssignStmtVarsPatternInfo() {
	vector<tuple<StmtIndex, VarIndex>> res;
	return res;
}

void Pattern::performCleanUp() {
	varPostFixTable = {};
	postFixVarTable = {};
}
