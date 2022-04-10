#include "./AffectsProcessor.h"

AffectsProcessor::AffectsProcessor() {
	this->affectsCache = new AffectsCache();
}

AffectsProcessor::AffectsProcessor(AffectsCache* affectsCache) {
	this->affectsCache = affectsCache;
}

AffectsProcessor::~AffectsProcessor() {
	delete affectsCache;
}

bool AffectsProcessor::isEarlyTerminationConditionFound(StmtIndex leftIdx, StmtIndex rightIdx, PKBGetter* pkbGetter) {
	return pkbGetter->getTypeFromStmtIdx(leftIdx) != StatementType::ASSIGN_TYPE
		|| pkbGetter->getTypeFromStmtIdx(rightIdx) != StatementType::ASSIGN_TYPE
		|| pkbGetter->getRSInfoFromLeftArg(RelationshipType::USES_S, rightIdx).size() <= 0
		|| pkbGetter->getProcFromStmt(leftIdx) != pkbGetter->getProcFromStmt(rightIdx);
}

bool AffectsProcessor::checkRsHoldsFromTraversal(StmtIndex leftIdx, StmtIndex rightIdx, PKBGetter* pkbGetter) {
	VarIndex modifiedVar = pkbGetter->getRSInfoFromLeftArg(RelationshipType::MODIFIES_S, leftIdx).front();
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

			// If modifiedVar is being modified by a non-container stmt, do not add it
			if (pkbGetter->getTypeFromStmtIdx(successor) != StatementType::IF_TYPE
				&& pkbGetter->getTypeFromStmtIdx(successor) != StatementType::WHILE_TYPE
				&& pkbGetter->getRSContainsInfo(RelationshipType::MODIFIES_S, successor, modifiedVar)) {
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

bool AffectsProcessor::doesRsHold(StmtIndex leftIdx, StmtIndex rightIdx, PKBGetter* pkbGetter) {
	if (isEarlyTerminationConditionFound(leftIdx, rightIdx, pkbGetter)) {
		return false;
	}

	VarIndex modifiedVar = pkbGetter->getRSInfoFromLeftArg(RelationshipType::MODIFIES_S, leftIdx).front();
	if (!pkbGetter->getRSContainsInfo(RelationshipType::USES_S, rightIdx, modifiedVar)) {
		return false; // ensures that rightStmt var indeed uses the modifiedVar
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

std::vector<StmtIndex> AffectsProcessor::computeStmtsFromIndex(StmtIndex index,
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

std::vector<StmtIndex> AffectsProcessor::getUsingLeftStmtIndex(StmtIndex leftIdx, PKBGetter* pkbGetter) {
	// if AffectsProcessor has already been computed FULLY for a leftIdx, use it directly
	if (affectsCache->isPredecessorFullyComputed(leftIdx)) {
		return affectsCache->getFromLeftArg(leftIdx);
	}

	return computeStmtsFromIndex(leftIdx,
		std::bind(&AffectsProcessor::doesRsHold, this, leftIdx, std::placeholders::_1, pkbGetter),
		std::bind(&PKBGetter::getRSInfoFromLeftArg, pkbGetter, RelationshipType::NEXT, std::placeholders::_1),
		std::bind(&AffectsCache::insertSuccessors, affectsCache, std::placeholders::_1, std::placeholders::_2));
};

std::vector<StmtIndex> AffectsProcessor::getUsingRightStmtIndex(StmtIndex rightIdx, PKBGetter* pkbGetter) {
	// if AffectsProcessor has already been computed FULLY for a rightIdx, use it directly
	if (affectsCache->isSuccessorFullyComputed(rightIdx)) {
		return affectsCache->getFromRightArg(rightIdx);
	}

	return computeStmtsFromIndex(rightIdx,
		std::bind(&AffectsProcessor::doesRsHold, this, std::placeholders::_1, rightIdx, pkbGetter),
		std::bind(&PKBGetter::getRSInfoFromRightArg, pkbGetter, RelationshipType::NEXT, std::placeholders::_1),
		std::bind(&AffectsCache::insertPredecessors, affectsCache, std::placeholders::_1, std::placeholders::_2));
};

std::tuple<std::vector<StmtIndex>, std::vector<StmtIndex>> AffectsProcessor::getAll(PKBGetter* pkbGetter) {
	std::vector<StmtIndex> stmtIndices = pkbGetter->getAllStmts();
	std::vector<StmtIndex> assignStmtIndices;

	for (StmtIndex stmtIdx : stmtIndices) {
		if (pkbGetter->getTypeFromStmtIdx(stmtIdx) == StatementType::ASSIGN_TYPE) {
			assignStmtIndices.push_back(stmtIdx);
		}
	}

	std::vector<StmtIndex> leftStmtIndices;
	std::vector<StmtIndex> rightStmtIndices;
	for (StmtIndex stmtIdx : assignStmtIndices) {
		auto affectedStmtsIndices = getUsingLeftStmtIndex(stmtIdx, pkbGetter);
		for (StmtIndex affectedStmtIdx : affectedStmtsIndices) {
			leftStmtIndices.push_back(stmtIdx);
			rightStmtIndices.push_back(affectedStmtIdx);
		}
	}

	return std::make_tuple(leftStmtIndices, rightStmtIndices);
};
