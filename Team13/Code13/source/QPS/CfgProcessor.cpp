#include "./CFGProcessor.h"

std::vector<StmtIndex> CFGProcessor::getVectorFromSet(std::unordered_set<StmtIndex> stmtIndicesSet) {
	return std::vector<StmtIndex>(stmtIndicesSet.begin(), stmtIndicesSet.end());
}
