#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "../Common/Types.h"

class Container {
protected:
	static std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> containerStmtTable;
	static std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> containedStmtTable;
	static void insert(StmtIndex& containerStmtIdx, std::unordered_set<StmtIndex, StmtIndex::HashFunction>& containedStmts);
	static std::unordered_set<StmtIndex, StmtIndex::HashFunction> getAllContainedStmts(StmtIndex& containerStmtIdx);

public:
	static void insertStmtInContainer(StmtIndex containerStmtIdx, StmtIndex containedStmtIdx);
	static std::unordered_set<StmtIndex, StmtIndex::HashFunction> getStmtsInContainer(StmtIndex& containerStmtIdx);
	static std::unordered_set<StmtIndex, StmtIndex::HashFunction> getContainersOfStmt(StmtIndex& containedStmtIdx);
	static void populate();
	static void performCleanUp();
};
