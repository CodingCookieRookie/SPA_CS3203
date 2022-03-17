#include <map>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "./Entity.h"

std::unordered_map<VarIndex, std::string> Entity::varNameTable;
std::unordered_map<ProcIndex, std::string> Entity::procNameTable;
std::unordered_set<ConstValue> Entity::constTable;
std::unordered_map<StmtIndex, StatementType> Entity::stmtTypeTable;
std::unordered_map<StatementType, std::unordered_set<StmtIndex>> Entity::stmtIdxFromTypeTable;
std::unordered_map<ProcIndex, std::unordered_set<StmtIndex>> Entity::procStmtTable;

size_t Entity::getVarTableSize() {
	return varNameTable.size();
}

size_t Entity::getProcTableSize() {
	return procNameTable.size();
}

size_t Entity::getStmtTypeTableSize() {
	return stmtTypeTable.size();
}

VarIndex Entity::insertVar(std::string varName) {
	NameIndex nameIdx = Attribute::insertNameValue(varName);

	if (!Attribute::containsVarName(varName)) {
		VarIndex varIdx = VarIndex(getVarTableSize() + 1);
		varNameTable[varIdx] = varName;
		Attribute::insertVarIdxByName(varIdx, nameIdx);
		return varIdx;
	}

	return getVarIdx(varName);
}

bool Entity::containsVar(std::string& varName) {
	if (!Attribute::containsName(varName)) return false;

	return Attribute::containsVarName(varName);
}

std::string Entity::getVarName(VarIndex varIdx) {
	return varNameTable[varIdx];
}

VarIndex Entity::getVarIdx(std::string varName) {
	std::unordered_set<VarIndex> varIdxSet = Attribute::getVarIdxSet(varName);
	return VarIndex(*varIdxSet.begin());
}

std::vector<VarIndex> Entity::getAllVars() {
	std::vector<VarIndex> res;

	for (auto& varInfo : varNameTable) {
		res.push_back(varInfo.first);
	}

	return res;
}

ProcIndex Entity::insertProc(std::string procName) {
	NameIndex nameIdx = Attribute::insertNameValue(procName);

	if (!Attribute::containsProcName(procName)) {
		ProcIndex procIdx = ProcIndex(getProcTableSize() + 1);
		procNameTable[procIdx] = procName;
		Attribute::insertProcIdxByName(procIdx, nameIdx);
		return procIdx;
	}

	return getProcIdx(procName);
}

bool Entity::containsProc(std::string& procName) {
	if (!Attribute::containsName(procName)) return false;

	return Attribute::containsProcName(procName);
}

std::string Entity::getProcName(ProcIndex procIdx) {
	return procNameTable[procIdx];
}

ProcIndex Entity::getProcIdx(std::string procName) {
	std::unordered_set<ProcIndex> procIdxSet = Attribute::getProcIdxSet(procName);
	return ProcIndex(*procIdxSet.begin());
}

std::vector<ProcIndex> Entity::getAllProcs() {
	std::vector<ProcIndex> res;

	for (auto& procInfo : procNameTable) {
		res.push_back(procInfo.first);
	}

	return res;
}

void Entity::insertConst(ConstValue constant) {
	constTable.insert(constant);
}

std::vector<ConstValue> Entity::getAllConsts() {
	std::vector<ConstValue> res;

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

StmtIndex Entity::insertStmt(StatementType stmtType, std::string& nameValue) {
	StmtIndex stmtIdx = insertStmt(stmtType);
	Attribute::insertStmtByName(stmtIdx, stmtType, nameValue);

	return stmtIdx;
}

bool Entity::isContainerStmt(StmtIndex& stmtIdx) {
	StatementType stmtType = stmtTypeTable[stmtIdx];
	return stmtType == StatementType::whileType || stmtType == StatementType::ifType;
}

bool Entity::containsStmt(StmtIndex stmtNo) {
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

	for (auto& stmtTypeEntry : stmtTypeTable) {
		res.push_back(stmtTypeEntry.first);
	}

	return res;
}

std::vector<StmtIndex> Entity::getAllContainerStmts() {
	std::vector<StmtIndex> res;

	for (auto& stmtTypeEntry : stmtTypeTable) {
		StmtIndex stmtIdx = stmtTypeEntry.first;
		if (Entity::isContainerStmt(stmtIdx)) {
			res.push_back(stmtTypeEntry.first);
		}
	}

	return res;
}

void Entity::insertStmtFromProc(ProcIndex procIdx, StmtIndex stmtIdx) {
	procStmtTable[procIdx].insert(stmtIdx);
}

std::unordered_set<StmtIndex> Entity::getStmtsFromProc(ProcIndex& procIdx) {
	return procStmtTable[procIdx];
}

void Entity::performCleanUp() {
	varNameTable = {};
	procNameTable = {};
	constTable = {};
	stmtTypeTable = {};
	stmtIdxFromTypeTable = {};
	procStmtTable = {};
}
