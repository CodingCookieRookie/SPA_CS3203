#include <string>
#include <unordered_set>
#include <unordered_map>
#include <tuple>
#include <vector>

#include "./Types.h";
#include "./Entity.h";

using namespace std;

unordered_map<int, string> Entity::varNameTable;
unordered_map<string, int> Entity::varIdxTable;
unordered_map<int, string> Entity::procNameTable;
unordered_map<string, int> Entity::procIdxTable;
unordered_set<int> Entity::constTable;
unordered_map<int, string> Entity::stmtTypeTable;
unordered_map<string, unordered_set<int>> Entity::stmtIdxFromTypeTable;
unordered_map<int, unordered_set<int>> Entity::procStmtTable;

int Entity::insertNow(VAR_IDX v) {
	return 1;
}

int Entity::insertNow(PROC_IDX v) {
	return 2;
}

int Entity::getVarTableSize() {
	return varNameTable.size();
}

int Entity::getProcTableSize() {
	return procNameTable.size();
}

int Entity::getStmtTypeTableSize() {
	return stmtTypeTable.size();
}

void Entity::insertVar(string varName) {
	int varIdx = getVarTableSize() + 1;
	varNameTable[varIdx] = varName;
	varIdxTable[varName] = varIdx;
}

string Entity::getVarName(int varIdx) {
	return varNameTable[varIdx];
}

int Entity::getVarIdx(string varName) {
	return varIdxTable[varName];
}

vector<string> Entity::getAllVars() {
	vector<string> res;
	
	for (auto& varInfo : varIdxTable)
		res.push_back(varInfo.first);

	return res;
}

void Entity::insertProc(string procName) {
	int procIdx = getProcTableSize() + 1;
	procNameTable[procIdx] = procName;
	procIdxTable[procName] = procIdx;
}

string Entity::getProcName(int procIdx) {
	return procNameTable[procIdx];
}

int Entity::getProcIdx(string procName) {
	return procIdxTable[procName];
}

vector<string> Entity::getAllProcs() {
	vector<string> res;

	for (auto& procInfo : procIdxTable)
		res.push_back(procInfo.first);

	return res;
}

void Entity::insertConst(int constant) {
	constTable.insert(constant);
}

vector<int> Entity::getAllConsts() {
	vector<int> res;

	for (auto& constant : constTable)
		res.push_back(constant);

	return res;
}

void Entity::insertStmt(string stmtType) {
	int stmtIdx = getStmtTypeTableSize() + 1;
	stmtTypeTable[stmtIdx] = stmtType;

	stmtIdxFromTypeTable[stmtType].insert(stmtIdx);
}

unordered_set<int> Entity::getStmtIdxFromType(string stmtType) {
	return stmtIdxFromTypeTable[stmtType];
}

vector<int> Entity::getAllStmts() {
	vector<int> res;
	int size = getStmtTypeTableSize();

	for (int i = 1; i <= size; i++) {
		res.push_back(i);
	}

	return res;
}

void Entity::insertStmtFromProc(int procIdx, int stmtIdx) {
	procStmtTable[procIdx].insert(stmtIdx);
}

unordered_set<int> Entity::getStmtsFromProc(int procIdx) {
	return procStmtTable[procIdx];
}