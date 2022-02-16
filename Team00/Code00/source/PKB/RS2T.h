#pragma once

#ifndef RS2T_H
#define RS2T_H

#include "./RS2.h"

template<class T, class U>
class RS2T : public RS2<RS2T<T, U>> {
private:
	using RS2<RS2T<T, U>>::insert;
	static std::unordered_set<StmtIndex, StmtIndex::HashFunction> getAllSuccessors(StmtIndex predecessor);
protected:
	RS2T();
public:
	static void populate();
};

template<class T, class U>
RS2T<T, U>::RS2T() {};

template<class T, class U>
std::unordered_set<StmtIndex, StmtIndex::HashFunction> RS2T<T, U>::getAllSuccessors(StmtIndex predecessor) {
	std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction>
		uPredSucTable = U::getPredSucTable();

	if (predSucTable.find(predecessor) != predSucTable.end()) {
		return predSucTable[predecessor];
	}

	std::unordered_set<StmtIndex, StmtIndex::HashFunction> successors;
	for (auto& successor : uPredSucTable[predecessor]) {
		successors.insert(successor);
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> grandSuccessors = getAllSuccessors(successor);
		for (auto& grandSuccessor : grandSuccessors) {
			successors.insert(grandSuccessor);
		}
	}

	return successors;
}

template<class T, class U>
void RS2T<T, U>::populate() {
	std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction>
		uPredSucTable = U::getPredSucTable();

	for (auto& uPredSucEntry : uPredSucTable) {
		StmtIndex predecessor = uPredSucEntry.first;
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> successors = getAllSuccessors(predecessor);
		for (auto& successor : successors) {
			insert(predecessor, successor);
		}
	}
};

#endif
