#include "./AffectsProcessor.h"

AffectsProcessor::AffectsProcessor() {
	this->affectsCache = new AffectsCache();
}

AffectsProcessor::AffectsProcessor(AffectsCache* affectsCache) {
	this->affectsCache = affectsCache;
}

AffectsProcessor::~AffectsProcessor() { affectsCache->performCleanUp(); }

bool AffectsProcessor::isBasicAffectsRequirementsFulfilled(StmtIndex leftIdx, StmtIndex rightIdx) {
	return Entity::getTypeFromStmtIdx(leftIdx) == StatementType::ASSIGN_TYPE
		&& Entity::getTypeFromStmtIdx(rightIdx) == StatementType::ASSIGN_TYPE
		&& UsesS::getVariables(rightIdx).size() > 0
		&& Entity::getProcFromStmt(leftIdx) == Entity::getProcFromStmt(rightIdx);
}

bool AffectsProcessor::checkRsHoldsFromTraversal(StmtIndex leftIdx, StmtIndex rightIdx) {
	VarIndex modifiedVar = ModifiesS::getVariables(leftIdx).front();
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

			// If modifiedVar is being modified by a non-container stmt, do not add it
			if (Entity::getTypeFromStmtIdx(successor) != StatementType::IF_TYPE
				&& Entity::getTypeFromStmtIdx(successor) != StatementType::WHILE_TYPE
				&& ModifiesS::contains(successor, modifiedVar)) {
				continue;
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

bool AffectsProcessor::doesRsHold(StmtIndex leftIdx, StmtIndex rightIdx) {
	if (!isBasicAffectsRequirementsFulfilled(leftIdx, rightIdx)) {
		return false;
	}

	VarIndex modifiedVar = ModifiesS::getVariables(leftIdx).front();
	if (!UsesS::contains(rightIdx, modifiedVar)) {
		return false; // ensures that rightStmt var indeed uses the modifiedVar
	}

	if (affectsCache->contains(leftIdx, rightIdx)) {
		return true;
	}

	if (affectsCache->isPredecessorFullyComputed(leftIdx)
		|| affectsCache->isSuccessorFullyComputed(rightIdx)) {
		return false;
	}

	return checkRsHoldsFromTraversal(leftIdx, rightIdx);
}

/*
Acts as a helper method for the getUsingLeftStmtIndex and getUsingRightStmtIndex.
Performs BFS to compute a vector of stmtIndices for which the Affects relationship holds
*/
std::vector<StmtIndex> AffectsProcessor::getStmtsFromComputationHelper(StmtIndex index,
	std::function<bool(StmtIndex)> doesRsHold,
	std::function<std::vector<StmtIndex>(StmtIndex&)> getSubsequentNextStmts,
	std::function<void(StmtIndex, std::unordered_set<StmtIndex>)> insertSubsequentAffectsStmts) {
	std::unordered_set<StmtIndex> allStmtsSet;
	std::unordered_set<StmtIndex> visited;
	std::queue<StmtIndex> queue;
	queue.push(index);

	while (!queue.empty()) {
		StmtIndex stmtIdx = queue.front();
		queue.pop();

		for (StmtIndex subsequentStmt : getSubsequentNextStmts(stmtIdx)) {
			if (doesRsHold(subsequentStmt)) {
				allStmtsSet.insert(subsequentStmt);
			}

			if (visited.find(subsequentStmt) == visited.end()) {
				visited.insert(subsequentStmt);
				queue.push(subsequentStmt);
			}
		}

		visited.insert(stmtIdx);
	}

	insertSubsequentAffectsStmts(index, allStmtsSet);
	return getVectorFromSet(allStmtsSet);
}

std::vector<StmtIndex> AffectsProcessor::getUsingLeftStmtIndex(StmtIndex leftIdx) {
	// if AffectsProcessor has already been computed FULLY for a leftIdx, use it directly
	if (affectsCache->isPredecessorFullyComputed(leftIdx)) {
		return affectsCache->getSuccessors(leftIdx);
	}

	return getStmtsFromComputationHelper(leftIdx,
		std::bind(&AffectsProcessor::doesRsHold, this, leftIdx, std::placeholders::_1),
		&Next::getSuccessors,
		std::bind(&AffectsCache::insertSuccessors, affectsCache, std::placeholders::_1, std::placeholders::_2));
};

std::vector<StmtIndex> AffectsProcessor::getUsingRightStmtIndex(StmtIndex rightIdx) {
	// if AffectsProcessor has already been computed FULLY for a rightIdx, use it directly
	if (affectsCache->isSuccessorFullyComputed(rightIdx)) {
		return affectsCache->getPredecessors(rightIdx);
	}

	return getStmtsFromComputationHelper(rightIdx,
		std::bind(&AffectsProcessor::doesRsHold, this, std::placeholders::_1, rightIdx),
		&Next::getPredecessors,
		std::bind(&AffectsCache::insertPredecessors, affectsCache, std::placeholders::_1, std::placeholders::_2));
};

std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> AffectsProcessor::getAll() {
	std::vector<StmtIndex> stmtIndices = Entity::getAllStmts();
	std::vector<StmtIndex> assignStmtIndices;

	for (StmtIndex stmtIdx : stmtIndices) {
		if (Entity::getTypeFromStmtIdx(stmtIdx) == StatementType::ASSIGN_TYPE) {
			assignStmtIndices.push_back(stmtIdx);
		}
	}

	std::vector<StmtIndex> leftStmtIndices;
	std::vector<StmtIndex> rightStmtIndices;
	for (StmtIndex stmtIdx : assignStmtIndices) {
		auto affectedStmtsIndices = getUsingLeftStmtIndex(stmtIdx);
		for (StmtIndex affectedStmtIdx : affectedStmtsIndices) {
			leftStmtIndices.push_back(stmtIdx);
			rightStmtIndices.push_back(affectedStmtIdx);
		}
	}

	return std::make_tuple(leftStmtIndices, rightStmtIndices);
};

void AffectsProcessor::performCleanUp() {
	affectsCache->performCleanUp();
}
