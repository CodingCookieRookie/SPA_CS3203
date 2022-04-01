#pragma once

#include <functional>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "../Common/Types.h"
#include "./CacheStorageTypes.h"

class CFGProcessor {
protected:
	std::vector<StmtIndex> getVectorFromSet(std::unordered_set<StmtIndex>);
	virtual bool checkRsHoldsFromTraversal(StmtIndex leftIdx, StmtIndex rightIdx) = 0;

public:
	virtual bool doesRsHold(StmtIndex leftIdx, StmtIndex rightIdx) = 0;
	virtual std::vector<StmtIndex> getUsingLeftStmtIndex(StmtIndex leftIdx) = 0;
	virtual std::vector<StmtIndex> getUsingRightStmtIndex(StmtIndex rightIdx) = 0;
	virtual std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> getAll() = 0;
	virtual void performCleanUp() = 0;
};
