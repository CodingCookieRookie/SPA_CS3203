#pragma once

#ifndef CONTAINER_H
#define CONTAINER_H

#include <string>
#include <unordered_set>
#include <unordered_map>

#include "../common/Types.h"

class Container {
protected:
	static std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> containerStmtTable;
	static std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> containedStmtTable;

public:
	static void insertStmtInContainer(StmtIndex containerStmtIdx, StmtIndex stmtIdx);
	static std::unordered_set<StmtIndex, StmtIndex::HashFunction> getStmtsInContainer(StmtIndex containerStmtIdx);
	static std::unordered_set<StmtIndex, StmtIndex::HashFunction> getContainersOfStmt(StmtIndex stmtIdx);
	static void populate();
	static void performCleanUp();
};

#endif
