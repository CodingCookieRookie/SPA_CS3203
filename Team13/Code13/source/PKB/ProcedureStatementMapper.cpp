#include "./ProcedureStatementMapper.h"

ProcedureStatementMapper::ProcedureStatementMapper() {}

void ProcedureStatementMapper::insertStmtFromProc(ProcIndex procIdx, StmtIndex stmtIdx) {
	procStmtBidirectionalTable.insert(procIdx, stmtIdx);
}

std::vector<StmtIndex> ProcedureStatementMapper::getStmtsFromProc(ProcIndex& procIdx) {
	return procStmtBidirectionalTable.getFromLeftArg(procIdx);
}

ProcIndex ProcedureStatementMapper::getProcFromStmt(StmtIndex stmtIdx) {
	return procStmtBidirectionalTable.getFromRightArg(stmtIdx);
};

std::tuple<std::vector<ProcIndex>, std::vector<StmtIndex>> ProcedureStatementMapper::getAllProcStmts() {
	return procStmtBidirectionalTable.getAll();
}
