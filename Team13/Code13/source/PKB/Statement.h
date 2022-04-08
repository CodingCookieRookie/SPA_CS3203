#pragma once

#include <vector>

#include "../Common/Types.h"
#include "./Attribute.h"
#include "./BidirectionalTable/BidirectionalTableOneWaySet.h"

/* Forward declaration */
class Attribute;

class Statement {
private:
	size_t getStmtTypeTableSize();
	bool isContainerStmt(StmtIndex& stmtIdx);
	BidirectionalTableOneWaySet<StatementType, StmtIndex> stmtBidirectionalTable;

public:
	Statement();
	~Statement();
	StmtIndex insertStmt(StatementType stmtType);
	StmtIndex insertStmt(StatementType stmtType, std::string& nameValue, Attribute* attribute);
	bool containsStmt(StmtIndex stmtNo);
	std::vector<StmtIndex> getStmtIdxFromType(StatementType stmtType);
	StatementType getTypeFromStmtIdx(StmtIndex stmtIdx);
	std::vector<StmtIndex> getAllStmts();
	std::vector<StmtIndex> getAllContainerStmts();
};
