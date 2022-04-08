#include "./NextTProcessor.h"

NextTProcessor::NextTProcessor(NextTCache* nextTCache) {
	this->nextTCache = nextTCache;
}

NextTProcessor::~NextTProcessor() { nextTCache->performCleanUp(); }

bool NextTProcessor::checkRsHoldsFromTraversal(StmtIndex leftIdx, StmtIndex rightIdx, PKBGetter* pkbGetter) {
	std::unordered_set<StmtIndex> visited;
	std::queue<StmtIndex> queue;
	queue.push(leftIdx);

	while (!queue.empty()) {
		StmtIndex stmtIdx = queue.front();
		queue.pop();

		for (StmtIndex successor : pkbGetter->getRSInfoFromLeftArg(RelationshipType::NEXT, stmtIdx)) {
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

bool NextTProcessor::doesRsHold(StmtIndex leftIdx, StmtIndex rightIdx, PKBGetter* pkbGetter) {
	// if NextTProcessor has already been FULLY computed for leftIdx, use it directly
	if (nextTCache->contains(leftIdx, rightIdx)) {
		return true;
	} else if (nextTCache->isPredecessorFullyComputed(leftIdx)
		|| nextTCache->isSuccessorFullyComputed(rightIdx)) {
		return false;
	}

	return checkRsHoldsFromTraversal(leftIdx, rightIdx, pkbGetter);
}

std::vector<StmtIndex> NextTProcessor::computeStmtsFromIndex(StmtIndex index,
	std::function<bool(StmtIndex)> checkIfFullyComputed,
	std::function<std::vector<StmtIndex>(StmtIndex)> getSubsequentCacheStmts,
	std::function<std::vector<StmtIndex>(StmtIndex&)> getSubsequentNextStmts,
	std::function<void(StmtIndex, std::unordered_set<StmtIndex>)> insertSubsequentNextStmts) {
	std::unordered_set<StmtIndex> visited;
	std::unordered_set<StmtIndex> allStmtsSet;
	std::queue<StmtIndex> queue;
	queue.push(index);

	while (!queue.empty()) {
		StmtIndex stmtIdx = queue.front();
		queue.pop();

		// if NextTProcessor has already been computed for a stmtIdx, use it directly
		if (checkIfFullyComputed(stmtIdx)) {
			for (auto subsequentStmt : getSubsequentCacheStmts(stmtIdx)) {
				allStmtsSet.insert(subsequentStmt);
			}
		} else { // if NextTProcessor has not been computed, compute it using Next
			for (StmtIndex subsequentStmt : getSubsequentNextStmts(stmtIdx)) {
				allStmtsSet.insert(subsequentStmt);

				if (visited.find(subsequentStmt) == visited.end()) {
					visited.insert(subsequentStmt);
					queue.push(subsequentStmt);
				}
			}
		}

		visited.insert(stmtIdx);
	}

	insertSubsequentNextStmts(index, allStmtsSet);
	return getVectorFromSet(allStmtsSet);
}

std::vector<StmtIndex> NextTProcessor::getUsingLeftStmtIndex(StmtIndex leftIdx, PKBGetter* pkbGetter) {
	// if NextTProcessor has already been computed FULLY for a leftIdx, use it directly
	if (nextTCache->isPredecessorFullyComputed(leftIdx)) {
		return nextTCache->getFromLeftArg(leftIdx);
	}

	return computeStmtsFromIndex(leftIdx,
		std::bind(&NextTCache::isPredecessorFullyComputed, nextTCache, std::placeholders::_1),
		std::bind(&NextTCache::getFromLeftArg, nextTCache, std::placeholders::_1),
		std::bind(&PKBGetter::getRSInfoFromLeftArg, pkbGetter, RelationshipType::NEXT, std::placeholders::_1),
		std::bind(&NextTCache::insertSuccessors, nextTCache, std::placeholders::_1, std::placeholders::_2));
};

std::vector<StmtIndex> NextTProcessor::getUsingRightStmtIndex(StmtIndex rightIdx, PKBGetter* pkbGetter) {
	// if NextTProcessor has already been computed FULLY for a rightIdx, use it directly
	if (nextTCache->isSuccessorFullyComputed(rightIdx)) {
		return nextTCache->getFromRightArg(rightIdx);
	}

	return computeStmtsFromIndex(rightIdx,
		std::bind(&NextTCache::isSuccessorFullyComputed, nextTCache, std::placeholders::_1),
		std::bind(&NextTCache::getFromRightArg, nextTCache, std::placeholders::_1),
		std::bind(&PKBGetter::getRSInfoFromRightArg, pkbGetter, RelationshipType::NEXT, std::placeholders::_1),
		std::bind(&NextTCache::insertPredecessors, nextTCache, std::placeholders::_1, std::placeholders::_2));
};

std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> NextTProcessor::getAll(PKBGetter* pkbGetter) {
	std::vector<StmtIndex> stmtIndices = pkbGetter->getAllStmts();

	std::vector<StmtIndex> leftStmtIndices;
	std::vector<StmtIndex> rightStmtIndices;
	for (auto stmtIdx : stmtIndices) {
		auto nextTStmtsIndices = getUsingLeftStmtIndex(stmtIdx, pkbGetter);
		for (StmtIndex nextTStmtIdx : nextTStmtsIndices) {
			leftStmtIndices.push_back(stmtIdx);
			rightStmtIndices.push_back(nextTStmtIdx);
		}
	}

	return std::make_tuple(leftStmtIndices, rightStmtIndices);
}

void NextTProcessor::performCleanUp() {
	nextTCache->performCleanUp();
}
