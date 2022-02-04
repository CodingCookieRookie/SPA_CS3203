#include <string>
#include <unordered_set>
#include <unordered_map>
#include <tuple>
#include <vector>

#include "./Entity.h";

using namespace std;

unordered_map<VarIndex, string, VarIndex::HashFunction> Entity::varNameTable;
unordered_map<string, VarIndex> Entity::varIdxTable;
unordered_map<ProcIndex, string, ProcIndex::HashFunction> Entity::procNameTable;
unordered_map<string, ProcIndex> Entity::procIdxTable;
unordered_set<int> Entity::constTable;
unordered_map<StmtIndex, StatementType, StmtIndex::HashFunction> Entity::stmtTypeTable;
unordered_map<StatementType, unordered_set<StmtIndex, StmtIndex::HashFunction>> Entity::stmtIdxFromTypeTable;
unordered_map<ProcIndex, unordered_set<StmtIndex, StmtIndex::HashFunction>, ProcIndex::HashFunction> Entity::procStmtTable;

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
	VarIndex varIdx = VarIndex(getVarTableSize() + 1);
	varNameTable[varIdx] = varName;
	varIdxTable[varName] = varIdx;
}

string Entity::getVarName(VarIndex varIdx) {
	return varNameTable[varIdx];
}

VarIndex Entity::getVarIdx(string varName) {
	return varIdxTable[varName];
}

vector<string> Entity::getAllVars() {
	vector<string> res;

	for (auto& varInfo : varIdxTable)
		res.push_back(varInfo.first);

	return res;
}

void Entity::insertProc(string procName) {
	ProcIndex procIdx = ProcIndex(getProcTableSize() + 1);
	procNameTable[procIdx] = procName;
	procIdxTable[procName] = procIdx;
}

string Entity::getProcName(ProcIndex procIdx) {
	return procNameTable[procIdx];
}

ProcIndex Entity::getProcIdx(string procName) {
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

StmtIndex Entity::insertStmt(StatementType stmtType) {
	StmtIndex stmtIdx = StmtIndex(getStmtTypeTableSize() + 1);
	stmtTypeTable[stmtIdx] = stmtType;

	stmtIdxFromTypeTable[stmtType].insert(stmtIdx);

	return stmtIdx;
}

unordered_set<StmtIndex, StmtIndex::HashFunction> Entity::getStmtIdxFromType(StatementType stmtType) {
	return stmtIdxFromTypeTable[stmtType];
}

vector<StmtIndex> Entity::getAllStmts() {
	vector<StmtIndex> res;

	for (auto& stmtIdx : stmtTypeTable)
		res.push_back(stmtIdx.first);

	return res;
}

void Entity::insertStmtFromProc(ProcIndex procIdx, StmtIndex stmtIdx) {
	procStmtTable[procIdx].insert(stmtIdx);
}

unordered_set<StmtIndex, StmtIndex::HashFunction> Entity::getStmtsFromProc(ProcIndex procIdx) {
	return procStmtTable[procIdx];
}

void Entity::performCleanUp() {
	varNameTable = {};
	varIdxTable = {};
	procNameTable = {};
	procIdxTable = {};
	constTable = {};
	stmtTypeTable = {};
	stmtIdxFromTypeTable = {};
	procStmtTable = {};
}
