#pragma once

#include <vector>

#include "../../Common/Types.h"
#include "../BidirectionalTable/BidirectionalTableOneWaySet.h"

class ProcedureStatementMapper {
private:
	BidirectionalTableOneWaySet<ProcIndex, StmtIndex> procStmtBidirectionalTable;

public:
	ProcedureStatementMapper();
	~ProcedureStatementMapper();
	void insertStmtFromProc(ProcIndex procIdx, StmtIndex stmtIdx);
	std::vector<StmtIndex> getStmtsFromProc(ProcIndex& procIdx);
	ProcIndex getProcFromStmt(StmtIndex stmtIdx);
	std::tuple<std::vector<ProcIndex>, std::vector<StmtIndex>> getAllProcStmts();
};
