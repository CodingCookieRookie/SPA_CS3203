#pragma once

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "../common/Types.h"

template<class T>
class RS2 {
protected:
	RS2();
	static std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>,
		StmtIndex::HashFunction> predSucTable;
	static std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>,
		StmtIndex::HashFunction> sucPredTable;

public:
	static void insert(StmtIndex predecessor, StmtIndex successors);
	static bool containsSuccessor(StmtIndex predecessor, StmtIndex successor);
	static bool containsPredecessor(StmtIndex predecessor, StmtIndex successor);
	static std::vector<int> getSuccessorStmts(StmtIndex predecessor);
	static std::vector<int> getPredecessorStmts(StmtIndex successor);
	static std::tuple<std::vector<int>, std::vector<int>> getAllPredecessorSuccessorInfo();
	static std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>,
		StmtIndex::HashFunction> getPredSucTable();
	static std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>,
		StmtIndex::HashFunction> getSucPredTable();
	static void performCleanUp();
};

template<class T> std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>,
	StmtIndex::HashFunction> RS2<T>::predSucTable = {};
template<class T> std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>,
	StmtIndex::HashFunction> RS2<T>::sucPredTable = {};

template<class T>
RS2<T>::RS2() {};

template<class T>
void RS2<T>::insert(StmtIndex predecessor, StmtIndex successor) {
	predSucTable[predecessor].insert(successor);
	sucPredTable[successor].insert(predecessor);
}

template<class T>
bool RS2<T>::containsPredecessor(StmtIndex predecessor, StmtIndex successor) {
	if (sucPredTable.find(successor) == sucPredTable.end()) {
		return false;
	}

	std::unordered_set<StmtIndex, StmtIndex::HashFunction> predecessors = sucPredTable[successor];
	return predecessors.find(predecessor) != predecessors.end();
};

template<class T>
bool RS2<T>::containsSuccessor(StmtIndex predecessor, StmtIndex successor) {
	if (predSucTable.find(predecessor) == predSucTable.end()) {
		return false;
	}

	std::unordered_set<StmtIndex, StmtIndex::HashFunction> successors = predSucTable[predecessor];
	return successors.find(successor) != successors.end();
};

template<class T>
std::vector<int> RS2<T>::getSuccessorStmts(StmtIndex predecessor) {
	std::vector<int> successors;
	for (auto& successor : predSucTable[predecessor]) {
		successors.push_back(successor.index);
	}
	return successors;
};

template<class T>
std::vector<int> RS2<T>::getPredecessorStmts(StmtIndex successor) {
	std::vector<int> predecessors;
	for (auto& predecessor : sucPredTable[successor]) {
		predecessors.push_back(predecessor.index);
	}
	return predecessors;
};

template<class T>
std::tuple<std::vector<int>, std::vector<int>> RS2<T>::getAllPredecessorSuccessorInfo() {
	std::vector<int> predecessors;
	std::vector<int> successors;
	for (auto& predSucEntry : predSucTable) {
		for (auto& successor : predSucEntry.second) {
			predecessors.push_back(predSucEntry.first.index);
			successors.push_back(successor.index);
		}
	}
	return std::make_tuple(predecessors, successors);
};

template<class T>
std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction>
RS2<T>::getPredSucTable() {
	std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> data;
	for (auto& predSucEntry : predSucTable) {
		for (auto& successor : predSucEntry.second) {
			data[predSucEntry.first].insert(successor);
		}
	}
	return data;
};

template<class T>
std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction>
RS2<T>::getSucPredTable() {
	std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> data;
	for (auto& sucPredEntry : sucPredTable) {
		for (auto& predecessor : sucPredEntry.second) {
			data[sucPredEntry.first].insert(predecessor);
		}
	}
	return data;
};

template<class T>
void RS2<T>::performCleanUp() {
	predSucTable = {};
	sucPredTable = {};
};
