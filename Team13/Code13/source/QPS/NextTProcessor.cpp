#include "./NextTProcessor.h"

NextTProcessor::NextTProcessor(NextTCache* nextTCache) {
	this->nextTCache = nextTCache;
}

NextTProcessor::~NextTProcessor() { nextTCache->performCleanUp(); }

bool NextTProcessor::checkRsHoldsFromTraversal(StmtIndex leftIdx, StmtIndex rightIdx) {
	std::unordered_set<StmtIndex> visited;
	std::queue<StmtIndex> queue;
	queue.push(leftIdx);

	while (!queue.empty()) {
		StmtIndex stmtIdx = queue.front();
		queue.pop();

		for (StmtIndex successor : Next::getSuccessors(stmtIdx)) {
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

bool NextTProcessor::doesRsHold(StmtIndex leftIdx, StmtIndex rightIdx) {
	// if NextTProcessor has already been FULLY computed for leftIdx, use it directly
	if (nextTCache->contains(leftIdx, rightIdx)) {
		return true;
	} else if (nextTCache->isPredecessorFullyComputed(leftIdx)
		|| nextTCache->isSuccessorFullyComputed(rightIdx)) {
		return false;
	}

	return checkRsHoldsFromTraversal(leftIdx, rightIdx);
}

/*
Acts as a helper method for the getUsingLeftStmtIndex and getUsingRightStmtIndex.
Performs BFS to compute a vector of stmtIndices for which the NextT relationship holds
*/
std::vector<StmtIndex> NextTProcessor::getStmtsFromComputationHelper(StmtIndex index,
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

std::vector<StmtIndex> NextTProcessor::getUsingLeftStmtIndex(StmtIndex leftIdx) {
	// if NextTProcessor has already been computed FULLY for a leftIdx, use it directly
	if (nextTCache->isPredecessorFullyComputed(leftIdx)) {
		return nextTCache->getSuccessors(leftIdx);
	}

	return getStmtsFromComputationHelper(leftIdx,
		std::bind(&NextTCache::isPredecessorFullyComputed, nextTCache, std::placeholders::_1),
		std::bind(&NextTCache::getSuccessors, nextTCache, std::placeholders::_1),
		&Next::getSuccessors,
		std::bind(&NextTCache::insertSuccessors, nextTCache, std::placeholders::_1, std::placeholders::_2));
};

std::vector<StmtIndex> NextTProcessor::getUsingRightStmtIndex(StmtIndex rightIdx) {
	// if NextTProcessor has already been computed FULLY for a rightIdx, use it directly
	if (nextTCache->isSuccessorFullyComputed(rightIdx)) {
		return nextTCache->getPredecessors(rightIdx);
	}

	return getStmtsFromComputationHelper(rightIdx,
		std::bind(&NextTCache::isSuccessorFullyComputed, nextTCache, std::placeholders::_1),
		std::bind(&NextTCache::getPredecessors, nextTCache, std::placeholders::_1),
		&Next::getPredecessors,
		std::bind(&NextTCache::insertPredecessors, nextTCache, std::placeholders::_1, std::placeholders::_2));
};

std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> NextTProcessor::getAll() {
	auto predSucTable = Next::getPredSucTable();

	std::vector<StmtIndex> leftStmtIndices;
	std::vector<StmtIndex> rightStmtIndices;
	for (auto entry : predSucTable) {
		StmtIndex leftIdx = entry.first;
		auto nextTStmtsIndices = getUsingLeftStmtIndex(leftIdx);
		for (StmtIndex nextTStmtIdx : nextTStmtsIndices) {
			leftStmtIndices.push_back(leftIdx);
			rightStmtIndices.push_back(nextTStmtIdx);
		}
	}

	return std::make_tuple(leftStmtIndices, rightStmtIndices);
}

void NextTProcessor::performCleanUp() {
	nextTCache->performCleanUp();
}
