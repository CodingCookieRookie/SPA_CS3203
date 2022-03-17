#pragma once

#include "./RS2.h"

template<class T, class U, typename SynonymIndex>
class RS2T : public RS2<RS2T<T, U, SynonymIndex>, SynonymIndex> {
private:
	using RS2<RS2T<T, U, SynonymIndex>, SynonymIndex>::insert;
	static std::unordered_set<SynonymIndex> getAllSuccessors(SynonymIndex& predecessor,
		std::unordered_map<SynonymIndex, std::unordered_set<SynonymIndex>> uPredSucTable);
protected:
	RS2T();
public:
	static void populate();
};

template<class T, class U, typename SynonymIndex>
RS2T<T, U, SynonymIndex>::RS2T() {};

template<class T, class U, typename SynonymIndex>
std::unordered_set<SynonymIndex> RS2T<T, U, SynonymIndex>::getAllSuccessors(SynonymIndex& predecessor,
	std::unordered_map<SynonymIndex, std::unordered_set<SynonymIndex>> uPredSucTable) {
	if (predSucTable.find(predecessor) != predSucTable.end()) {
		return predSucTable[predecessor];
	}

	std::unordered_set<SynonymIndex> successors;
	for (auto successor : uPredSucTable[predecessor]) {
		successors.insert(successor);
		std::unordered_set<SynonymIndex> grandSuccessors = getAllSuccessors(successor, uPredSucTable);
		for (auto grandSuccessor : grandSuccessors) {
			successors.insert(grandSuccessor);
			insert(successor, grandSuccessor);
		}
	}

	return successors;
}

template<class T, class U, typename SynonymIndex>
void RS2T<T, U, SynonymIndex>::populate() {
	auto uPredSucTable = U::getPredSucTable();
	if (uPredSucTable.size() == 0) {
		return;
	}

	for (auto uPredSucEntry : uPredSucTable) {
		SynonymIndex predecessor = uPredSucEntry.first;
		if (predSucTable.find(predecessor) != predSucTable.end()) {
			continue;
		}

		std::unordered_set<SynonymIndex> successors = getAllSuccessors(predecessor, uPredSucTable);
		for (auto successor : successors) {
			insert(predecessor, successor);
		}
	}
};
