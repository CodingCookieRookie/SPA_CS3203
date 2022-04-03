#include <map>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "./Entity.h"

BidirectionalIndexTable<VarIndex> Entity::varIdxBidirectionalTable;
BidirectionalIndexTable<ProcIndex> Entity::procIdxBidirectionalTable;
std::unordered_set<ConstValue> Entity::constTable;
BidirectionalTableOneWaySet<StatementType, StmtIndex> Entity::stmtBidirectionalTable;
BidirectionalTableOneWaySet<ProcIndex, StmtIndex> Entity::procStmtBidirectionalTable;

size_t Entity::getStmtTypeTableSize() {
	return stmtBidirectionalTable.getSize();
}

VarIndex Entity::insertVar(std::string varName) {
	NameIndex nameIdx = Attribute::insertNameValue(varName);

	if (!varIdxBidirectionalTable.contains(varName)) {
		VarIndex varIdx = varIdxBidirectionalTable.insert(varName);
		Attribute::insertVarIdxByName(varIdx, nameIdx);
		return varIdx;
	}

	return getVarIdx(varName);
}

bool Entity::containsVar(std::string& varName) {
	return varIdxBidirectionalTable.contains(varName);
}

std::string Entity::getVarName(VarIndex varIdx) {
	return varIdxBidirectionalTable.getStringFromIndex(varIdx);
}

VarIndex Entity::getVarIdx(std::string varName) {
	return varIdxBidirectionalTable.getIndexFromString(varName);
}

std::vector<VarIndex> Entity::getAllVars() {
	return varIdxBidirectionalTable.getAll();
}

ProcIndex Entity::insertProc(std::string procName) {
	NameIndex nameIdx = Attribute::insertNameValue(procName);

	if (!procIdxBidirectionalTable.contains(procName)) {
		ProcIndex procIdx = procIdxBidirectionalTable.insert(procName);
		Attribute::insertProcIdxByName(procIdx, nameIdx);
		return procIdx;
	}

	return getProcIdx(procName);
}

bool Entity::containsProc(std::string& procName) {
	return procIdxBidirectionalTable.contains(procName);
}

std::string Entity::getProcName(ProcIndex procIdx) {
	return procIdxBidirectionalTable.getStringFromIndex(procIdx);
}

ProcIndex Entity::getProcIdx(std::string procName) {
	return procIdxBidirectionalTable.getIndexFromString(procName);
}

std::vector<ProcIndex> Entity::getAllProcs() {
	return procIdxBidirectionalTable.getAll();
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
	stmtBidirectionalTable.insert(stmtType, stmtIdx);

	return stmtIdx;
}

StmtIndex Entity::insertStmt(StatementType stmtType, std::string& nameValue) {
	StmtIndex stmtIdx = insertStmt(stmtType);
	Attribute::insertStmtByName(stmtIdx, stmtType, nameValue);

	return stmtIdx;
}

bool Entity::isContainerStmt(StmtIndex& stmtIdx) {
	StatementType stmtType = stmtBidirectionalTable.getFromRightArg(stmtIdx);
	return stmtType == StatementType::WHILE_TYPE || stmtType == StatementType::IF_TYPE;
}

bool Entity::containsStmt(StmtIndex stmtNo) {
	return stmtBidirectionalTable.contains(stmtNo);
}

std::vector<StmtIndex> Entity::getStmtIdxFromType(StatementType stmtType) {
	return stmtBidirectionalTable.getFromLeftArg(stmtType);
}

StatementType Entity::getTypeFromStmtIdx(StmtIndex stmtIdx) {
	return stmtBidirectionalTable.getFromRightArg(stmtIdx);
};

std::vector<StmtIndex> Entity::getAllStmts() {
	std::vector<StmtIndex> res;

	for (size_t stmtIdx = 1; stmtIdx <= getStmtTypeTableSize(); stmtIdx++) {
		res.push_back(StmtIndex(stmtIdx));
	}

	return res;
}

std::vector<StmtIndex> Entity::getAllContainerStmts() {
	std::vector<StmtIndex> res;

	std::vector<StmtIndex> stmts = getAllStmts();

	for (auto& stmtIdx : stmts) {
		if (Entity::isContainerStmt(stmtIdx)) {
			res.push_back(stmtIdx);
		}
	}

	return res;
}

void Entity::insertStmtFromProc(ProcIndex procIdx, StmtIndex stmtIdx) {
	procStmtBidirectionalTable.insert(procIdx, stmtIdx);
}

std::vector<StmtIndex> Entity::getStmtsFromProc(ProcIndex& procIdx) {
	return procStmtBidirectionalTable.getFromLeftArg(procIdx);
}

ProcIndex Entity::getProcFromStmt(StmtIndex stmtIdx) {
	return procStmtBidirectionalTable.getFromRightArg(stmtIdx);
};

std::tuple<std::vector<ProcIndex>, std::vector<StmtIndex>> Entity::getAllProcStmts() {
	return procStmtBidirectionalTable.getAll();
}

void Entity::performCleanUp() {
	varIdxBidirectionalTable = BidirectionalIndexTable<VarIndex>();
	procIdxBidirectionalTable = BidirectionalIndexTable<ProcIndex>();
	constTable = {};
	stmtBidirectionalTable = BidirectionalTableOneWaySet<StatementType, StmtIndex>();
	procStmtBidirectionalTable = BidirectionalTableOneWaySet<ProcIndex, StmtIndex>();
}
