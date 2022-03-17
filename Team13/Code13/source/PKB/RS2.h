#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../Common/Types.h"

template<class T, typename SynonymIndex>
class RS2 {
protected:
	RS2();
	static std::unordered_map<SynonymIndex, std::unordered_set<SynonymIndex>> predSucTable;
	static std::unordered_map<SynonymIndex, std::unordered_set<SynonymIndex>> sucPredTable;

public:
	static void insert(SynonymIndex& predecessor, SynonymIndex& successor);
	static bool containsSuccessor(SynonymIndex& predecessor, SynonymIndex& successor);
	static bool containsPredecessor(SynonymIndex& predecessor, SynonymIndex& successor);
	static std::vector<SynonymIndex> getSuccessors(SynonymIndex& predecessor);
	static std::vector<SynonymIndex> getPredecessors(SynonymIndex& successor);
	static std::tuple<std::vector<SynonymIndex>, std::vector<SynonymIndex>> getAllPredecessorSuccessorInfo();
	static std::unordered_map<SynonymIndex, std::unordered_set<SynonymIndex>> getPredSucTable();
	static void performCleanUp();
};

template<class T, typename SynonymIndex>
std::unordered_map<SynonymIndex, std::unordered_set<SynonymIndex>> RS2<T, SynonymIndex>::predSucTable = {};

template<class T, typename SynonymIndex>
std::unordered_map<SynonymIndex, std::unordered_set<SynonymIndex>> RS2<T, SynonymIndex>::sucPredTable = {};

template<class T, typename SynonymIndex>
RS2<T, SynonymIndex>::RS2() {};

template<class T, typename SynonymIndex>
void RS2<T, SynonymIndex>::insert(SynonymIndex& predecessor, SynonymIndex& successor) {
	predSucTable[predecessor].insert(successor);
	sucPredTable[successor].insert(predecessor);
}

template<class T, typename SynonymIndex>
bool RS2<T, SynonymIndex>::containsPredecessor(SynonymIndex& predecessor, SynonymIndex& successor) {
	if (sucPredTable.find(successor) == sucPredTable.end()) {
		return false;
	}

	std::unordered_set<SynonymIndex>predecessors = sucPredTable[successor];
	return predecessors.find(predecessor) != predecessors.end();
};

template<class T, typename SynonymIndex>
bool RS2<T, SynonymIndex>::containsSuccessor(SynonymIndex& predecessor, SynonymIndex& successor) {
	if (predSucTable.find(predecessor) == predSucTable.end()) {
		return false;
	}

	std::unordered_set<SynonymIndex>successors = predSucTable[predecessor];
	return successors.find(successor) != successors.end();
};

template<class T, typename SynonymIndex>
std::vector<SynonymIndex> RS2<T, SynonymIndex>::getSuccessors(SynonymIndex& predecessor) {
	std::vector<SynonymIndex> successors;
	for (auto successor : predSucTable[predecessor]) {
		successors.push_back(successor);
	}
	return successors;
};

template<class T, typename SynonymIndex>
std::vector<SynonymIndex> RS2<T, SynonymIndex>::getPredecessors(SynonymIndex& successor) {
	std::vector<SynonymIndex> predecessors;
	for (auto predecessor : sucPredTable[successor]) {
		predecessors.push_back(predecessor);
	}
	return predecessors;
};

template<class T, typename SynonymIndex>
std::tuple<std::vector<SynonymIndex>, std::vector<SynonymIndex>> RS2<T, SynonymIndex>::getAllPredecessorSuccessorInfo() {
	std::vector<SynonymIndex> predecessors;
	std::vector<SynonymIndex> successors;
	for (auto predSucEntry : predSucTable) {
		for (auto successor : predSucEntry.second) {
			predecessors.push_back(predSucEntry.first);
			successors.push_back(successor);
		}
	}
	return std::make_tuple(predecessors, successors);
};

template<class T, typename SynonymIndex>
std::unordered_map<SynonymIndex, std::unordered_set<SynonymIndex>> RS2<T, SynonymIndex>::getPredSucTable() {
	std::unordered_map<SynonymIndex, std::unordered_set<SynonymIndex>> data;
	for (auto predSucEntry : predSucTable) {
		for (auto successor : predSucEntry.second) {
			data[predSucEntry.first].insert(successor);
		}
	}
	return data;
};

template<class T, typename SynonymIndex>
void RS2<T, SynonymIndex>::performCleanUp() {
	predSucTable = {};
	sucPredTable = {};
};
