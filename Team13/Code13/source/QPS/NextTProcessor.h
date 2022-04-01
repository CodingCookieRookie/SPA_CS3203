#pragma once

#include "./CFGProcessor.h"
#include "./CacheStorage.h"
#include "../PKB/Next.h"

class NextTProcessor : public CFGProcessor {
protected:
	NextTCache* nextTCache;
	bool checkRsHoldsFromTraversal(StmtIndex leftIdx, StmtIndex rightIdx) override;
	std::vector<StmtIndex> getStmtsFromComputationHelper(StmtIndex index,
		std::function<bool(StmtIndex)> checkIfFullyComputed,
		std::function<std::vector<StmtIndex>(StmtIndex)> getSubsequentCacheStmts,
		std::function<std::vector<StmtIndex>(StmtIndex&)> getSubsequentNextStmts,
		std::function<void(StmtIndex, std::unordered_set<StmtIndex>)> insertSubsequentNextStmts);

public:
	NextTProcessor(NextTCache* nextTCache);
	~NextTProcessor();
	bool doesRsHold(StmtIndex leftIdx, StmtIndex rightIdx) override;
	std::vector<StmtIndex> getUsingLeftStmtIndex(StmtIndex leftIdx) override;
	std::vector<StmtIndex> getUsingRightStmtIndex(StmtIndex rightIdx) override;
	std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> getAll() override;
	void performCleanUp() override;
};
