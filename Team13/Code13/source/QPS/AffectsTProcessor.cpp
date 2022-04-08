#include "./AffectsTProcessor.h"

AffectsTProcessor::AffectsTProcessor(AffectsTCache* affectsTCache, AffectsProcessor* affectsProcessor) {
	this->affectsTCache = affectsTCache;
	this->affectsProcessor = affectsProcessor;
}

AffectsTProcessor::~AffectsTProcessor() {
	affectsTCache->performCleanUp();
	affectsProcessor->performCleanUp();
}

bool AffectsTProcessor::checkRsHoldsFromTraversal(StmtIndex leftIdx, StmtIndex rightIdx, PKBGetter* pkbGetter) {
	std::unordered_set<StmtIndex> visited;
	std::queue<StmtIndex> queue;
	queue.push(leftIdx);

	while (!queue.empty()) {
		StmtIndex stmtIdx = queue.front();
		queue.pop();

		std::vector<StmtIndex> affectedStmts = affectsProcessor->getUsingLeftStmtIndex(stmtIdx, pkbGetter);
		for (StmtIndex successor : affectedStmts) {
			if (successor == rightIdx) {
				return true;
			}

			if (visited.find(successor) == visited.end()) {
				visited.insert(successor);
				queue.push(successor);
			}
		}

		visited.insert(stmtIdx);
	}

	return false;
}

bool AffectsTProcessor::doesRsHold(StmtIndex leftIdx, StmtIndex rightIdx, PKBGetter* pkbGetter) {
	if (isEarlyTerminationConditionFound(leftIdx, rightIdx, pkbGetter)) {
		return false;
	}

	if (affectsCache->contains(leftIdx, rightIdx)) {
		return true;
	}

	if (affectsCache->isPredecessorFullyComputed(leftIdx)
		|| affectsCache->isSuccessorFullyComputed(rightIdx)) {
		return false;
	}

	return checkRsHoldsFromTraversal(leftIdx, rightIdx, pkbGetter);
}
