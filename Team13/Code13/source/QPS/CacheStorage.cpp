#include "./CacheStorage.h"

void CacheStorage::insert(StmtIndex predecessor, StmtIndex successor) {
	predSucTable[predecessor].insert(successor);
	sucPredTable[successor].insert(predecessor);
}

void CacheStorage::insertSuccessors(StmtIndex predecessor, std::unordered_set<StmtIndex> successors) {
	for (StmtIndex successor : successors) {
		insert(predecessor, successor);
	}
	fullyComputedPredecessors.insert(predecessor);
}

void CacheStorage::insertPredecessors(StmtIndex successor, std::unordered_set<StmtIndex> predecessors) {
	for (StmtIndex predecessor : predecessors) {
		insert(predecessor, successor);
	}
	fullyComputedSuccessors.insert(successor);
}

bool CacheStorage::isPredecessorFullyComputed(StmtIndex predecessor) {
	return fullyComputedPredecessors.find(predecessor) != fullyComputedPredecessors.end();
}

bool CacheStorage::isSuccessorFullyComputed(StmtIndex successor) {
	return fullyComputedSuccessors.find(successor) != fullyComputedSuccessors.end();
}

bool CacheStorage::contains(StmtIndex predecessor, StmtIndex successor) {
	if (predSucTable.find(predecessor) == predSucTable.end()) {
		return false;
	}

	std::unordered_set<StmtIndex>successors = predSucTable[predecessor];
	return successors.find(successor) != successors.end();
};

std::vector<StmtIndex> CacheStorage::getFromLeftArg(StmtIndex predecessor) {
	std::vector<StmtIndex> successors;
	for (auto successor : predSucTable[predecessor]) {
		successors.push_back(successor);
	}
	return successors;
};

std::vector<StmtIndex> CacheStorage::getFromRightArg(StmtIndex successor) {
	std::vector<StmtIndex> predecessors;
	for (auto predecessor : sucPredTable[successor]) {
		predecessors.push_back(predecessor);
	}
	return predecessors;
};

void CacheStorage::performCleanUp() {
	predSucTable = {};
	sucPredTable = {};
	fullyComputedPredecessors = {};
	fullyComputedSuccessors = {};
};
