#pragma once

#ifndef RS1_H
#define RS1_H

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "../common/Types.h"

using namespace std; //TO REMOVE

template<class T>
class RS1 {
protected:
	RS1();
	static std::unordered_map<VarIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, VarIndex::HashFunction> varStmtTable;
	static std::unordered_map<StmtIndex, std::unordered_set<VarIndex, VarIndex::HashFunction>, StmtIndex::HashFunction> stmtVarTable;
	static std::unordered_map<VarIndex, std::unordered_set<ProcIndex, ProcIndex::HashFunction>, VarIndex::HashFunction> varProcTable;
	static std::unordered_map<ProcIndex, std::unordered_set<VarIndex, VarIndex::HashFunction>, ProcIndex::HashFunction> procVarTable;

public:
	static void insert(StmtIndex  stmtIndex, VarIndex varIndex);
	static void insert(ProcIndex procIndex, VarIndex varIndex);
	static bool contains(StmtIndex  stmtIndex, VarIndex varIndex);
	static bool contains(ProcIndex procIndex, VarIndex varIndex);
	static std::unordered_set<StmtIndex, StmtIndex::HashFunction> getStatements(VarIndex varIndex);
	static std::unordered_set<ProcIndex, ProcIndex::HashFunction> getProcedures(VarIndex varIndex);
	static std::unordered_set<VarIndex, VarIndex::HashFunction> getVariables(StmtIndex  stmtIndex);
	static std::unordered_set<VarIndex, VarIndex::HashFunction> getVariables(ProcIndex procIndex);
	static std::vector<std::tuple<ProcIndex, VarIndex>> getAllProcVarInfo();
	static std::vector<std::tuple<StmtIndex, VarIndex>> getAllStmtVarInfo();
	static void performCleanUp();
};

template<class T> std::unordered_map<VarIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, VarIndex::HashFunction> RS1<T>::varStmtTable = {};
template<class T> std::unordered_map<StmtIndex, std::unordered_set<VarIndex, VarIndex::HashFunction>, StmtIndex::HashFunction> RS1<T>::stmtVarTable = {};
template<class T> std::unordered_map<VarIndex, std::unordered_set<ProcIndex, ProcIndex::HashFunction>, VarIndex::HashFunction> RS1<T>::varProcTable = {};
template<class T> std::unordered_map<ProcIndex, std::unordered_set<VarIndex, VarIndex::HashFunction>, ProcIndex::HashFunction> RS1<T>::procVarTable = {};

template<class T>
RS1<T>::RS1() {};

template<class T>
void RS1<T>::insert(StmtIndex  stmtIndex, VarIndex varIndex) {
	varStmtTable[varIndex].insert(stmtIndex);
	stmtVarTable[stmtIndex].insert(varIndex);
};

template<class T>
void RS1<T>::insert(ProcIndex procIndex, VarIndex varIndex) {
	varProcTable[varIndex].insert(procIndex);
	procVarTable[procIndex].insert(varIndex);
};

template<class T>
bool RS1<T>::contains(StmtIndex  stmtIndex, VarIndex varIndex) {
	if (stmtVarTable.find(stmtIndex) == stmtVarTable.end()) {
		return false;
	}

	unordered_set<VarIndex, VarIndex::HashFunction> variables = stmtVarTable[stmtIndex];
	if (variables.find(varIndex) == variables.end()) {
		return false;
	}

	return true;
};

template<class T>
bool RS1<T>::contains(ProcIndex procIndex, VarIndex varIndex) {
	if (procVarTable.find(procIndex) == procVarTable.end()) {
		return false;
	}

	unordered_set<VarIndex, VarIndex::HashFunction> variables = procVarTable[procIndex];
	if (variables.find(varIndex) == variables.end()) {
		return false;
	}

	return true;
};

template<class T>
std::unordered_set<StmtIndex, StmtIndex::HashFunction> RS1<T>::getStatements(VarIndex varIndex) {
	return varStmtTable[varIndex];
};

template<class T>
std::unordered_set<ProcIndex, ProcIndex::HashFunction> RS1<T>::getProcedures(VarIndex varIndex) {
	return varProcTable[varIndex];
};

template<class T>
std::unordered_set<VarIndex, VarIndex::HashFunction> RS1<T>::getVariables(StmtIndex  stmtIndex) {
	return stmtVarTable[stmtIndex];
};

template<class T>
std::unordered_set<VarIndex, VarIndex::HashFunction> RS1<T>::getVariables(ProcIndex procIndex) {
	return procVarTable[procIndex];
};

template<class T>
std::vector<std::tuple<ProcIndex, VarIndex>> RS1<T>::getAllProcVarInfo() {
	vector<tuple<ProcIndex, VarIndex>> data;
	for (auto& procVarEntry : procVarTable) {
		for (auto& varIndex : procVarEntry.second) {
			data.push_back(make_tuple(procVarEntry.first, varIndex));
		}
	}
	return data;
};

template<class T>
std::vector<std::tuple<StmtIndex, VarIndex>> RS1<T>::getAllStmtVarInfo() {
	vector<tuple<StmtIndex, VarIndex>> data;
	for (auto& stmtVarEntry : stmtVarTable) {
		for (auto& varIndex : stmtVarEntry.second) {
			data.push_back(make_tuple(stmtVarEntry.first, varIndex));
		}
	}
	return data;
};

template<class T>
void RS1<T>::performCleanUp() {
	varStmtTable = {};
	stmtVarTable = {};
	varProcTable = {};
	procVarTable = {};
}

#endif
