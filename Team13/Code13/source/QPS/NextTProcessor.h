#pragma once

#include "./OnTheFlyRSProcessor.h"

class NextTProcessor : public OnTheFlyRSProcessor {
protected:
	NextTCache* nextTCache;
	bool checkRsHoldsFromTraversal(StmtIndex leftIdx, StmtIndex rightIdx, PKBGetter* pkbGetter) override;
	std::vector<StmtIndex> computeStmtsFromIndex(StmtIndex index,
		std::function<bool(StmtIndex)> checkIfFullyComputed,
		std::function<std::vector<StmtIndex>(StmtIndex)> getSubsequentCacheStmts,
		std::function<std::vector<StmtIndex>(StmtIndex&)> getSubsequentNextStmts,
		std::function<void(StmtIndex, std::unordered_set<StmtIndex>)> insertSubsequentNextStmts);

public:
	NextTProcessor(NextTCache* nextTCache);
	~NextTProcessor();
	bool doesRsHold(StmtIndex leftIdx, StmtIndex rightIdx, PKBGetter* pkbGetter) override;
	std::vector<StmtIndex> getUsingLeftStmtIndex(StmtIndex leftIdx, PKBGetter* pkbGetter) override;
	std::vector<StmtIndex> getUsingRightStmtIndex(StmtIndex rightIdx, PKBGetter* pkbGetter) override;
	std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> getAll(PKBGetter* pkbGetter) override;
	void performCleanUp() override;
};
