#pragma once

#ifndef RS2_H
#define RS2_H

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "../common/Types.h"

template<class T>
class RS2 {
protected:
	RS2();
	static std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> predSucTable;
	static std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> sucPredTable;

public:
	static bool containsSuccessor(StmtIndex predecessor, StmtIndex successor);
	static bool containsPredecessor(StmtIndex predecessor, StmtIndex successor);
	static std::unordered_set<StmtIndex, StmtIndex::HashFunction> getSuccessorStmts(StmtIndex predecessor);
	static std::unordered_set<StmtIndex, StmtIndex::HashFunction> getPredecessorStmts(StmtIndex successor);
	static std::vector<std::tuple<StmtIndex, StmtIndex>> getAllPredecessorSuccessorInfo();
	static std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> getPredSucTable();
	static std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> getSucPredTable();
	static void performCleanUp();
};

template<class T> std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> RS2<T>::predSucTable = {};
template<class T> std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction> RS2<T>::sucPredTable = {};

template<class T>
RS2<T>::RS2() {};

template<class T>
bool RS2<T>::containsPredecessor(StmtIndex predecessor, StmtIndex successor) {
	if (sucPredTable.find(successor) == sucPredTable.end()) {
		return false;
	}

	unordered_set<StmtIndex, StmtIndex::HashFunction> predecessors = sucPredTable[successor];
	return predecessors.find(predecessor) != predecessors.end();
};

template<class T>
bool RS2<T>::containsSuccessor(StmtIndex predecessor, StmtIndex successor) {
	if (predSucTable.find(predecessor) == predSucTable.end()) {
		return false;
	}

	unordered_set<StmtIndex, StmtIndex::HashFunction> successors = predSucTable[predecessor];
	return successors.find(successor) != successors.end();
};

template<class T>
std::unordered_set<StmtIndex, StmtIndex::HashFunction> RS2<T>::getSuccessorStmts(StmtIndex predecessor) {
	return predSucTable[predecessor];
};

template<class T>
std::unordered_set<StmtIndex, StmtIndex::HashFunction> RS2<T>::getPredecessorStmts(StmtIndex successor) {
	return sucPredTable[successor];
};

template<class T>
std::vector<std::tuple<StmtIndex, StmtIndex>> RS2<T>::getAllPredecessorSuccessorInfo() {
	vector<tuple<StmtIndex, StmtIndex>> data;
	for (auto& predSucEntry : predSucTable) {
		for (auto& successor : predSucEntry.second) {
			data.push_back(make_tuple(predSucEntry.first, successor));
		}
	}
	return data;
};

template<class T>
std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction>
RS2<T>::getPredSucTable() {
	return predSucTable;
};

template<class T>
std::unordered_map<StmtIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, StmtIndex::HashFunction>
RS2<T>::getSucPredTable() {
	return sucPredTable;
};

template<class T>
void RS2<T>::performCleanUp() {
	predSucTable = {};
	sucPredTable = {};
};

#endif
