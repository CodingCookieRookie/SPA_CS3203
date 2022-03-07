#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "./Entity.h"

std::unordered_map<VarIndex, std::string> Entity::varNameTable;
std::unordered_map<std::string, VarIndex> Entity::varIdxTable;
std::unordered_map<ProcIndex, std::string> Entity::procNameTable;
std::unordered_map<std::string, ProcIndex> Entity::procIdxTable;
std::unordered_set<int> Entity::constTable;
std::unordered_map<StmtIndex, StatementType> Entity::stmtTypeTable;
std::unordered_map<StatementType, std::unordered_set<StmtIndex>> Entity::stmtIdxFromTypeTable;
std::unordered_map<ProcIndex, std::unordered_set<StmtIndex>> Entity::procStmtTable;

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

bool Entity::containsVar(std::string varName) {
	return varIdxTable.find(varName) != varIdxTable.end();
}

std::string Entity::getVarName(VarIndex varIdx) {
	return varNameTable[varIdx];
}

VarIndex Entity::getVarIdx(std::string varName) {
	return varIdxTable[varName];
}

std::vector<VarIndex> Entity::getAllVars() {
	std::vector<VarIndex> res;

	for (auto& varInfo : varNameTable) {
		res.push_back(varInfo.first);
	}

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

bool Entity::containsProc(std::string procName) {
	return procIdxTable.find(procName) != procIdxTable.end();
}

std::string Entity::getProcName(ProcIndex procIdx) {
	return procNameTable[procIdx];
}

ProcIndex Entity::getProcIdx(std::string procName) {
	return procIdxTable[procName];
}

std::vector<ProcIndex> Entity::getAllProcs() {
	std::vector<ProcIndex> res;

	for (auto& procInfo : procNameTable) {
		res.push_back(procInfo.first);
	}

	return res;
}

void Entity::insertConst(int constant) {
	constTable.insert(constant);
}

std::vector<int> Entity::getAllConsts() {
	std::vector<int> res;

	for (auto& constant : constTable) {
		res.push_back(constant);
	}

	return res;
}

StmtIndex Entity::insertStmt(StatementType stmtType) {
	StmtIndex stmtIdx = StmtIndex(getStmtTypeTableSize() + 1);
	stmtTypeTable[stmtIdx] = stmtType;
	stmtIdxFromTypeTable[stmtType].insert(stmtIdx);

	return stmtIdx;
}

bool Entity::isContainerStmt(StmtIndex stmtIdx) {
	StatementType stmtType = stmtTypeTable[stmtIdx];
	return stmtType == StatementType::whileType || stmtType == StatementType::ifType;
}

bool Entity::containsStmt(int stmtNo) {
	return stmtTypeTable.find(StmtIndex(stmtNo)) != stmtTypeTable.end();
}

std::vector<StmtIndex> Entity::getStmtIdxFromType(StatementType stmtType) {
	std::vector<StmtIndex> res;

	for (auto& stmtIdx : stmtIdxFromTypeTable[stmtType]) {
		res.push_back(stmtIdx);
	}

	return res;
}

std::vector<StmtIndex> Entity::getAllStmts() {
	std::vector<StmtIndex> res;

	for (auto& stmtIdx : stmtTypeTable) {
		res.push_back(stmtIdx.first);
	}

	return res;
}

void Entity::insertStmtFromProc(ProcIndex procIdx, StmtIndex stmtIdx) {
	procStmtTable[procIdx].insert(stmtIdx);
}

std::unordered_set<StmtIndex> Entity::getStmtsFromProc(ProcIndex procIdx) {
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
