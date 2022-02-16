#pragma once

#ifndef RS2T_H
#define RS2T_H

#include "./RS2.h"

template<class T>
class RS2T : public RS2<RS2T<T>> {
private:
	using RS2<RS2T>::insert;
	static std::unordered_set<StmtIndex, StmtIndex::HashFunction> getAllSuccessors(StmtIndex predecessor, std::unordered_map<StmtIndex,
		std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable);
protected:
	RS2T();
public:
	static void populate(std::unordered_map<StmtIndex,
		std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable);
};

template<class T>
RS2T<T>::RS2T() {};

template<class T>
std::unordered_set<StmtIndex, StmtIndex::HashFunction> RS2T<T>::getAllSuccessors(StmtIndex predecessor,
	std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> uPredSucTable) {
	if (predSucTable.find(predecessor) != predSucTable.end()) {
		return predSucTable[predecessor];
	}

	std::unordered_set<StmtIndex, StmtIndex::HashFunction> successors;
	for (auto& successor : uPredSucTable[predecessor]) {
		successors.insert(successor);
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> grandSuccessors = getAllSuccessors(successor, uPredSucTable);
		for (auto& grandSuccessor : grandSuccessors) {
			successors.insert(grandSuccessor);
		}
	}

	return successors;
}

template<class T>
void RS2T<T>::populate(std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>,
	StmtIndex::HashFunction> uPredSucTable) {
	for (auto& uPredSucEntry : uPredSucTable) {
		StmtIndex predecessor = uPredSucEntry.first;
		std::unordered_set<StmtIndex, StmtIndex::HashFunction> successors = getAllSuccessors(predecessor, uPredSucTable);
		for (auto& successor : successors) {
			insert(predecessor, successor);
		}
	}
};

#endif
