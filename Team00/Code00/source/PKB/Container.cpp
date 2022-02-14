#include <unordered_set>
#include <unordered_map>

#include "./Container.h"

std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> Container::containerStmtTable;
std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> Container::containedStmtTable;

void Container::insertStmtInContainer(StmtIndex containerStmtIdx, StmtIndex containedStmtIdx) {
	if (containerStmtTable.find(containerStmtIdx) == containerStmtTable.end()) {
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> containedStmtSet;
		containedStmtSet.insert(containedStmtIdx);
		containerStmtTable[containerStmtIdx] = containedStmtSet;
	}
	else {
		containerStmtTable[containerStmtIdx].insert(containedStmtIdx);
	}

	if (containedStmtTable.find(containedStmtIdx) == containedStmtTable.end()) {
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> containerStmtSet;
		containerStmtSet.insert(containerStmtIdx);
		containedStmtTable[containedStmtIdx] = containerStmtSet;
	}
	else {
		containedStmtTable[containedStmtIdx].insert(containerStmtIdx);
	}
}

std::unordered_set<StmtIndex, StmtIndex::HashFunction> Container::getStmtsInContainer(StmtIndex containerStmtIdx) {
	if (containerStmtTable.find(containerStmtIdx) == containerStmtTable.end()) {
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> stmtSet;
		return stmtSet;
	}
	else {
		return containerStmtTable[containerStmtIdx];
	}
}

std::unordered_set<StmtIndex, StmtIndex::HashFunction> Container::getContainersOfStmt(StmtIndex containedStmtIdx) {
	if (containedStmtTable.find(containedStmtIdx) == containedStmtTable.end()) {
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> stmtSet;
		return stmtSet;
	}
	else {
		return containedStmtTable[containedStmtIdx];
	}
}

void Container::insert(StmtIndex containerStmtIdx, std::unordered_set<StmtIndex, StmtIndex::HashFunction> containedStmts) {
	for (auto& containedStmtIdx : containedStmts) {
		containerStmtTable[containerStmtIdx].insert(containedStmtIdx);
		containedStmtTable[containedStmtIdx].insert(containerStmtIdx);
	}
};

std::unordered_set<StmtIndex, StmtIndex::HashFunction> Container::getAllContainedStmts(StmtIndex containerStmtIdx) {
	if (containerStmtTable.find(containerStmtIdx) == containerStmtTable.end()) {
		return containerStmtTable[containerStmtIdx];
	}

	std::unordered_set<StmtIndex, StmtIndex::HashFunction> containedStmts;
	for (auto& containedStmtIdx : containerStmtTable[containerStmtIdx]) {
		containedStmts.insert(containedStmtIdx);
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> grandContainedStmts = getAllContainedStmts(containedStmtIdx);
		for (auto& grandContainedStmtIdx : grandContainedStmts) {
			containedStmts.insert(grandContainedStmtIdx);
		}
	}

	return containedStmts;
}

void Container::populate() {
	for (auto& containerEntry : containerStmtTable) {
		StmtIndex containerStmtIdx = containerEntry.first;
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> containedStmts = getAllContainedStmts(containerStmtIdx);
		insert(containerStmtIdx, containedStmts);
	}
}

void Container::performCleanUp() {
	containerStmtTable = {};
	containedStmtTable = {};
}
