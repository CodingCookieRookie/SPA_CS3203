#include <string>
#include <unordered_set>
#include <unordered_map>
#include <tuple>
#include <vector>

#include "./Entity.h"

std::unordered_map<VarIndex, std::string, VarIndex::HashFunction> Entity::varNameTable;
std::unordered_map<std::string, VarIndex> Entity::varIdxTable;
std::unordered_map<ProcIndex, std::string, ProcIndex::HashFunction> Entity::procNameTable;
std::unordered_map<std::string, ProcIndex> Entity::procIdxTable;
std::unordered_set<int> Entity::constTable;
std::unordered_map<StmtIndex, StatementType, StmtIndex::HashFunction> Entity::stmtTypeTable;
std::unordered_map<StatementType, std::unordered_set<StmtIndex, StmtIndex::HashFunction>> Entity::stmtIdxFromTypeTable;
std::unordered_map<ProcIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, ProcIndex::HashFunction> Entity::procStmtTable;

int Entity::getVarTableSize() {
	return varNameTable.size();
}

int Entity::getProcTableSize() {
	return procNameTable.size();
}

int Entity::getStmtTypeTableSize() {
	return stmtTypeTable.size();
}

VarIndex Entity::insertVar(std::string varName) {
	if (varIdxTable.find(varName) == varIdxTable.end()) {
		VarIndex varIdx = VarIndex(getVarTableSize() + 1);
		varNameTable[varIdx] = varName;
		varIdxTable[varName] = varIdx;
	}
	return varIdxTable[varName];
}

std::string Entity::getVarName(VarIndex varIdx) {
	return varNameTable[varIdx];
}

VarIndex Entity::getVarIdx(std::string varName) {
	return varIdxTable[varName];
}

std::vector<std::string> Entity::getAllVars() {
	std::vector<std::string> res;

	for (auto& varInfo : varIdxTable)
		res.push_back(varInfo.first);

	return res;
}

ProcIndex Entity::insertProc(std::string procName) {
	if (procIdxTable.find(procName) == procIdxTable.end()) {
		ProcIndex procIdx = ProcIndex(getProcTableSize() + 1);
		procNameTable[procIdx] = procName;
		procIdxTable[procName] = procIdx;
	}
	return procIdxTable[procName];
}

std::string Entity::getProcName(ProcIndex procIdx) {
	return procNameTable[procIdx];
}

ProcIndex Entity::getProcIdx(std::string procName) {
	return procIdxTable[procName];
}

std::vector<std::string> Entity::getAllProcs() {
	std::vector<std::string> res;

	for (auto& procInfo : procIdxTable)
		res.push_back(procInfo.first);

	return res;
}

void Entity::insertConst(int constant) {
	constTable.insert(constant);
}

std::vector<int> Entity::getAllConsts() {
	std::vector<int> res;

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

std::unordered_set<StmtIndex, StmtIndex::HashFunction> Entity::getStmtIdxFromType(StatementType stmtType) {
	return stmtIdxFromTypeTable[stmtType];
}

std::vector<StmtIndex> Entity::getAllStmts() {
	std::vector<StmtIndex> res;

	for (auto& stmtIdx : stmtTypeTable)
		res.push_back(stmtIdx.first);

	return res;
}

void Entity::insertStmtFromProc(ProcIndex procIdx, StmtIndex stmtIdx) {
	procStmtTable[procIdx].insert(stmtIdx);
}

std::unordered_set<StmtIndex, StmtIndex::HashFunction> Entity::getStmtsFromProc(ProcIndex procIdx) {
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
