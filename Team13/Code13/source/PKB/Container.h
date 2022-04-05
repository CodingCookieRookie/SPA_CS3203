#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "../Common/Types.h"

class Container {
private:
	static std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> containerStmtTable;
	static std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> containedStmtTable;
	static void insert(StmtIndex& containerStmtIdx, std::unordered_set<StmtIndex>& containedStmts);
	static std::unordered_set<StmtIndex> getAllContainedStmts(StmtIndex& containerStmtIdx);

public:
	static void insertStmtInContainer(StmtIndex& containerStmtIdx, StmtIndex& containedStmtIdx);
	static std::unordered_set<StmtIndex> getStmtsInContainer(StmtIndex& containerStmtIdx);
	static std::unordered_set<StmtIndex> getContainersOfStmt(StmtIndex& containedStmtIdx);
	static void populate();
	static void performCleanUp();
};
