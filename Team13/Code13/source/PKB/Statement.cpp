#include "./Statement.h"

Statement::Statement() {}

size_t Statement::getStmtTypeTableSize() {
	return stmtBidirectionalTable.getSize();
}

StmtIndex Statement::insertStmt(StatementType stmtType) {
	StmtIndex stmtIdx = StmtIndex(getStmtTypeTableSize() + 1);
	stmtBidirectionalTable.insert(stmtType, stmtIdx);

	return stmtIdx;
}

StmtIndex Statement::insertStmt(StatementType stmtType, std::string& nameValue) {
	StmtIndex stmtIdx = insertStmt(stmtType);
	Attribute::insertStmtByName(stmtIdx, stmtType, nameValue);

	return stmtIdx;
}

bool Statement::containsStmt(StmtIndex stmtNo) {
	return stmtBidirectionalTable.contains(stmtNo);
}

bool Statement::isContainerStmt(StmtIndex& stmtIdx) {
	StatementType stmtType = stmtBidirectionalTable.getFromRightArg(stmtIdx);
	return stmtType == StatementType::WHILE_TYPE || stmtType == StatementType::IF_TYPE;
}

std::vector<StmtIndex> Statement::getStmtIdxFromType(StatementType stmtType) {
	return stmtBidirectionalTable.getFromLeftArg(stmtType);
}

StatementType Statement::getTypeFromStmtIdx(StmtIndex stmtIdx) {
	return stmtBidirectionalTable.getFromRightArg(stmtIdx);
};

std::vector<StmtIndex> Statement::getAllStmts() {
	std::vector<StmtIndex> res;

	for (size_t stmtIdx = 1; stmtIdx <= getStmtTypeTableSize(); stmtIdx++) {
		res.push_back(StmtIndex(stmtIdx));
	}

	return res;
}

std::vector<StmtIndex> Statement::getAllContainerStmts() {
	std::vector<StmtIndex> res;

	std::vector<StmtIndex> stmts = getAllStmts();

	for (auto& stmtIdx : stmts) {
		if (Statement::isContainerStmt(stmtIdx)) {
			res.push_back(stmtIdx);
		}
	}

	return res;
}
