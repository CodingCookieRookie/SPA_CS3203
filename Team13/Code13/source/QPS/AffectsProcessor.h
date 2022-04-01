#include "./CFGProcessor.h"
#include "./CacheStorage.h"
#include "../PKB/Next.h"
#include "../PKB/Entity.h"
#include "../PKB/UsesS.h"
#include "../PKB/ModifiesS.h"

class AffectsProcessor : public CFGProcessor {
protected:
	AffectsCache* affectsCache;
	bool isBasicAffectsRequirementsFulfilled(StmtIndex leftIdx, StmtIndex rightIdx);
	bool checkRsHoldsFromTraversal(StmtIndex leftIdx, StmtIndex rightIdx) override;
	std::vector<StmtIndex> getStmtsFromComputationHelper(StmtIndex index,
		std::function<bool(StmtIndex)> doesRsHold,
		std::function<std::vector<StmtIndex>(StmtIndex&)> getSubsequentNextStmts,
		std::function<void(StmtIndex, std::unordered_set<StmtIndex>)> insertSubsequentAffectsStmts);

public:
	AffectsProcessor();
	AffectsProcessor(AffectsCache* affectsCache);
	~AffectsProcessor();
	bool doesRsHold(StmtIndex leftIdx, StmtIndex rightIdx) override;
	std::vector<StmtIndex> getUsingLeftStmtIndex(StmtIndex leftIdx) override;
	std::vector<StmtIndex> getUsingRightStmtIndex(StmtIndex rightIdx) override;
	std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> getAll() override;
	void performCleanUp() override;
};
