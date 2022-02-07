#include "./FollowsT.h"

void FollowsT::insert(StmtIndex predecessor, std::unordered_set<StmtIndex, StmtIndex::HashFunction> successors) {
	for (auto& successor : successors) {
		predSucTable[predecessor].insert(successor);
		sucPredTable[successor].insert(predecessor);
	}
};

std::unordered_set<StmtIndex, StmtIndex::HashFunction> FollowsT::getAllSuccessors(StmtIndex predecessor,
	std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> followsPredSucTable) {
	if (predSucTable.find(predecessor) != predSucTable.end()) {
		return predSucTable[predecessor];
	}

	std::unordered_set<StmtIndex, StmtIndex::HashFunction> successors;
	for (auto& successor : followsPredSucTable[predecessor]) {
		successors.insert(successor);
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> grandSuccessors = getAllSuccessors(successor, followsPredSucTable);
		for (auto& grandSuccessor : grandSuccessors) {
			successors.insert(grandSuccessor);
		}
	}

	return successors;
}

void FollowsT::generate() {
	std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction>
		followsPredSucTable = Follows::getPredSucTable();

	for (auto& followsPredSucEntry : followsPredSucTable) {
		StmtIndex predecessor = followsPredSucEntry.first;
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> successors = getAllSuccessors(predecessor, followsPredSucTable);
		insert(predecessor, successors);
	}
};
