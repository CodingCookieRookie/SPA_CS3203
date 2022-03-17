#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../Common/Types.h"

template<class T, typename Index>
class RS2 {
protected:
	RS2();
	static std::unordered_map<Index, std::unordered_set<Index>> predSucTable;
	static std::unordered_map<Index, std::unordered_set<Index>> sucPredTable;

public:
	static void insert(Index& predecessor, Index& successor);
	static bool containsSuccessor(Index& predecessor, Index& successor);
	static bool containsPredecessor(Index& predecessor, Index& successor);
	static std::vector<int> getSuccessors(Index& predecessor);
	static std::vector<int> getPredecessors(Index& successor);
	static std::tuple<std::vector<int>, std::vector<int>> getAllPredecessorSuccessorInfo();
	static std::unordered_map<Index, std::unordered_set<Index>> getPredSucTable();
	static void performCleanUp();
};

template<class T, typename Index>
std::unordered_map<Index, std::unordered_set<Index>> RS2<T, Index>::predSucTable = {};

template<class T, typename Index>
std::unordered_map<Index, std::unordered_set<Index>> RS2<T, Index>::sucPredTable = {};

template<class T, typename Index>
RS2<T, Index>::RS2() {};

template<class T, typename Index>
void RS2<T, Index>::insert(Index& predecessor, Index& successor) {
	predSucTable[predecessor].insert(successor);
	sucPredTable[successor].insert(predecessor);
}

template<class T, typename Index>
bool RS2<T, Index>::containsPredecessor(Index& predecessor, Index& successor) {
	if (sucPredTable.find(successor) == sucPredTable.end()) {
		return false;
	}

	std::unordered_set<Index>predecessors = sucPredTable[successor];
	return predecessors.find(predecessor) != predecessors.end();
};

template<class T, typename Index>
bool RS2<T, Index>::containsSuccessor(Index& predecessor, Index& successor) {
	if (predSucTable.find(predecessor) == predSucTable.end()) {
		return false;
	}

	std::unordered_set<Index>successors = predSucTable[predecessor];
	return successors.find(successor) != successors.end();
};

template<class T, typename Index>
std::vector<int> RS2<T, Index>::getSuccessors(Index& predecessor) {
	std::vector<int> successors;
	for (auto successor : predSucTable[predecessor]) {
		successors.push_back(successor);
	}
	return successors;
};

template<class T, typename Index>
std::vector<int> RS2<T, Index>::getPredecessors(Index& successor) {
	std::vector<int> predecessors;
	for (auto predecessor : sucPredTable[successor]) {
		predecessors.push_back(predecessor);
	}
	return predecessors;
};

template<class T, typename Index>
std::tuple<std::vector<int>, std::vector<int>> RS2<T, Index>::getAllPredecessorSuccessorInfo() {
	std::vector<int> predecessors;
	std::vector<int> successors;
	for (auto predSucEntry : predSucTable) {
		for (auto successor : predSucEntry.second) {
			predecessors.push_back(predSucEntry.first);
			successors.push_back(successor);
		}
	}
	return std::make_tuple(predecessors, successors);
};

template<class T, typename Index>
std::unordered_map<Index, std::unordered_set<Index>> RS2<T, Index>::getPredSucTable() {
	std::unordered_map<Index, std::unordered_set<Index>> data;
	for (auto predSucEntry : predSucTable) {
		for (auto successor : predSucEntry.second) {
			data[predSucEntry.first].insert(successor);
		}
	}
	return data;
};

template<class T, typename Index>
void RS2<T, Index>::performCleanUp() {
	predSucTable = {};
	sucPredTable = {};
};
