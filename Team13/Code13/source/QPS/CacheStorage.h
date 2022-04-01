#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../Common/Types.h"

class CacheStorage {
protected:
	std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> predSucTable;
	std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> sucPredTable;
	std::unordered_set<StmtIndex> fullyComputedPredecessors;
	std::unordered_set<StmtIndex> fullyComputedSuccessors;
	void insert(StmtIndex predecessor, StmtIndex successor);

public:
	void insertSuccessors(StmtIndex predecessor, std::unordered_set<StmtIndex> successors);
	void insertPredecessors(StmtIndex successor, std::unordered_set<StmtIndex> predecessors);
	bool contains(StmtIndex predecessor, StmtIndex successor);
	bool isPredecessorFullyComputed(StmtIndex predecessor);
	bool isSuccessorFullyComputed(StmtIndex predecessor);
	std::vector<StmtIndex> getSuccessors(StmtIndex predecessor);
	std::vector<StmtIndex> getPredecessors(StmtIndex successors);
	void performCleanUp();
};
