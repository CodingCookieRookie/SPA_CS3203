#pragma once

#include "./AffectsProcessor.h"

class AffectsTProcessor : public AffectsProcessor {
protected:
	AffectsProcessor* affectsProcessor;
	AffectsTCache* affectsTCache;
	bool checkRsHoldsFromTraversal(StmtIndex leftIdx, StmtIndex rightIdx, PKBGetter* pkbGetter) override;

public:
	AffectsTProcessor(AffectsTCache* affectsTCache, AffectsProcessor* affectsProcessor);
	~AffectsTProcessor();
	bool doesRsHold(StmtIndex leftIdx, StmtIndex rightIdx, PKBGetter* pkbGetter) override;
};
