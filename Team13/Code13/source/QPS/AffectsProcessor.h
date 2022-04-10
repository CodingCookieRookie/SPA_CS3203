#pragma once

#include "./OnTheFlyRSProcessor.h"

class AffectsProcessor : public OnTheFlyRSProcessor {
protected:
	AffectsCache* affectsCache;
	bool isEarlyTerminationConditionFound(StmtIndex leftIdx, StmtIndex rightIdx, PKBGetter* pkbGetter);
	bool checkRsHoldsFromTraversal(StmtIndex leftIdx, StmtIndex rightIdx, PKBGetter* pkbGetter) override;
	std::vector<StmtIndex> computeStmtsFromIndex(StmtIndex index,
		std::function<bool(StmtIndex)> doesRsHold,
		std::function<std::vector<StmtIndex>(StmtIndex&)> getSubsequentNextStmts,
		std::function<void(StmtIndex, std::unordered_set<StmtIndex>)> insertSubsequentAffectsStmts);

public:
	AffectsProcessor();
	AffectsProcessor(AffectsCache* affectsCache);
	~AffectsProcessor();
	bool doesRsHold(StmtIndex leftIdx, StmtIndex rightIdx, PKBGetter* pkbGetter) override;
	std::vector<StmtIndex> getUsingLeftStmtIndex(StmtIndex leftIdx, PKBGetter* pkbGetter) override;
	std::vector<StmtIndex> getUsingRightStmtIndex(StmtIndex rightIdx, PKBGetter* pkbGetter) override;
	std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> getAll(PKBGetter* pkbGetter) override;
};
