#include <unordered_set>
#include <unordered_map>

#include "./Container.h"

std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> containerStmtTable;
std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> containedStmtTable;

void Container::insertStmtInContainer(StmtIndex containerStmtIdx, StmtIndex stmtIdx) {
}

std::unordered_set<StmtIndex, StmtIndex::HashFunction> Container::getStmtsInContainer(StmtIndex containerStmtIdx) {
	std::unordered_set<StmtIndex, StmtIndex::HashFunction> stmtSet;
	return stmtSet;
}

std::unordered_set<StmtIndex, StmtIndex::HashFunction> Container::getContainersOfStmt(StmtIndex stmtIdx) {
	std::unordered_set<StmtIndex, StmtIndex::HashFunction> stmtSet;
	return stmtSet;
}

void Container::populate() {
}

void Container::performCleanUp() {
	containerStmtTable = {};
	containedStmtTable = {};
}
