#pragma once

#include <functional>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "../../Common/Types.h"
#include "../../PKB/PKBGetter.h"
#include "CacheStorageTypes.h"
#include "CacheStorage.h"

class OnTheFlyRSProcessor {
protected:
	std::vector<StmtIndex> getVectorFromSet(std::unordered_set<StmtIndex>);
	virtual bool checkRsHoldsFromTraversal(StmtIndex leftIdx, StmtIndex rightIdx, PKBGetter* pkbGetter) = 0;

public:
	virtual bool doesRsHold(StmtIndex leftIdx, StmtIndex rightIdx, PKBGetter* pkbGetter) = 0;
	virtual std::vector<StmtIndex> getUsingLeftStmtIndex(StmtIndex leftIdx, PKBGetter* pkbGetter) = 0;
	virtual std::vector<StmtIndex> getUsingRightStmtIndex(StmtIndex rightIdx, PKBGetter* pkbGetter) = 0;
	virtual std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> getAll(PKBGetter* pkbGetter) = 0;
};
