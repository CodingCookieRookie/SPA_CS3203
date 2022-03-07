#include <unordered_map>
#include <unordered_set>

#include "./Container.h"

std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> Container::containerStmtTable;
std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> Container::containedStmtTable;

void Container::insertStmtInContainer(StmtIndex& containerStmtIdx, StmtIndex& containedStmtIdx) {
	containerStmtTable[containerStmtIdx].insert(containedStmtIdx);
	containedStmtTable[containedStmtIdx].insert(containerStmtIdx);
}

std::unordered_set<StmtIndex> Container::getStmtsInContainer(StmtIndex& containerStmtIdx) {
	return containerStmtTable[containerStmtIdx];
}

std::unordered_set<StmtIndex> Container::getContainersOfStmt(StmtIndex& containedStmtIdx) {
	return containedStmtTable[containedStmtIdx];
}

void Container::insert(StmtIndex& containerStmtIdx, std::unordered_set<StmtIndex>& containedStmts) {
	for (auto& containedStmtIdx : containedStmts) {
		containerStmtTable[containerStmtIdx].insert(containedStmtIdx);
		containedStmtTable[containedStmtIdx].insert(containerStmtIdx);
	}
};

std::unordered_set<StmtIndex> Container::getAllContainedStmts(StmtIndex& containerStmtIdx) {
	std::unordered_set<StmtIndex> containedStmts;
	for (auto containedStmtIdx : containerStmtTable[containerStmtIdx]) {
		containedStmts.insert(containedStmtIdx);
		std::unordered_set<StmtIndex> grandContainedStmts = getAllContainedStmts(containedStmtIdx);
		for (auto grandContainedStmtIdx : grandContainedStmts) {
			containedStmts.insert(grandContainedStmtIdx);
		}
	}

	return containedStmts;
}

void Container::populate() {
	for (auto& containerEntry : containerStmtTable) {
		StmtIndex containerStmtIdx = containerEntry.first;
		std::unordered_set<StmtIndex> containedStmts = getAllContainedStmts(containerStmtIdx);
		insert(containerStmtIdx, containedStmts);
	}
}

void Container::performCleanUp() {
	containerStmtTable = {};
	containedStmtTable = {};
}
