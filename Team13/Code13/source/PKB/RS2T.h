#pragma once

#include "./RS2.h"

template<class T, class U, typename Index>
class RS2T : public RS2<RS2T<T, U, Index>, Index> {
private:
	using RS2<RS2T<T, U, Index>, Index>::insert;
	static std::unordered_set<Index> getAllSuccessors(Index& predecessor,
		std::unordered_map<Index, std::unordered_set<Index>> uPredSucTable);
protected:
	RS2T();
public:
	static void populate();
};

template<class T, class U, typename Index>
RS2T<T, U, Index>::RS2T() {};

template<class T, class U, typename Index>
std::unordered_set<Index> RS2T<T, U, Index>::getAllSuccessors(Index& predecessor,
	std::unordered_map<Index, std::unordered_set<Index>> uPredSucTable) {
	if (predSucTable.find(predecessor) != predSucTable.end()) {
		return predSucTable[predecessor];
	}

	std::unordered_set<Index> successors;
	for (auto successor : uPredSucTable[predecessor]) {
		successors.insert(successor);
		std::unordered_set<Index> grandSuccessors = getAllSuccessors(successor, uPredSucTable);
		for (auto grandSuccessor : grandSuccessors) {
			successors.insert(grandSuccessor);
			insert(successor, grandSuccessor);
		}
	}

	return successors;
}

template<class T, class U, typename Index>
void RS2T<T, U, Index>::populate() {
	auto uPredSucTable = U::getPredSucTable();
	if (uPredSucTable.size() == 0) {
		return;
	}

	for (auto uPredSucEntry : uPredSucTable) {
		Index predecessor = uPredSucEntry.first;
		if (predSucTable.find(predecessor) != predSucTable.end()) {
			continue;
		}

		std::unordered_set<Index> successors = getAllSuccessors(predecessor, uPredSucTable);
		for (auto successor : successors) {
			insert(predecessor, successor);
		}
	}
};
