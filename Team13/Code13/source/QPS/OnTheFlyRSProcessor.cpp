#include "./OnTheFlyRSProcessor.h"

std::vector<StmtIndex> OnTheFlyRSProcessor::getVectorFromSet(std::unordered_set<StmtIndex> stmtIndicesSet) {
	return std::vector<StmtIndex>(stmtIndicesSet.begin(), stmtIndicesSet.end());
}
