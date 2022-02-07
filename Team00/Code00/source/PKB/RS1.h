#pragma once

#ifndef RS1_H
#define RS1_H

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "../common/Types.h"

template<class T>
class RS1 {
protected:
	RS1();
	static unordered_map<VarIndex, unordered_set<StmtIndex, StmtIndex::HashFunction>, VarIndex::HashFunction> varStmtTable;
	static unordered_map<StmtIndex, unordered_set<VarIndex, VarIndex::HashFunction>, StmtIndex::HashFunction> stmtVarTable;
	static unordered_map<VarIndex, unordered_set<ProcIndex, ProcIndex::HashFunction>, VarIndex::HashFunction> varProcTable;
	static unordered_map<ProcIndex, unordered_set<VarIndex, VarIndex::HashFunction>, ProcIndex::HashFunction> procVarTable;

public:
	static void insert(StmtIndex  stmtIndex, VarIndex varIndex);
	static void insert(ProcIndex procIndex, VarIndex varIndex);
	static bool contains(StmtIndex  stmtIndex, VarIndex varIndex);
	static bool contains(ProcIndex procIndex, VarIndex varIndex);
	static unordered_set<StmtIndex, StmtIndex::HashFunction> getStatements(VarIndex varIndex);
	static unordered_set<ProcIndex, ProcIndex::HashFunction> getProcedures(VarIndex varIndex);
	static unordered_set<VarIndex, VarIndex::HashFunction> getVariables(StmtIndex  stmtIndex);
	static unordered_set<VarIndex, VarIndex::HashFunction> getVariables(ProcIndex procIndex);
	static vector<tuple<ProcIndex, VarIndex>> getAllProcVarInfo();
	static vector<tuple<StmtIndex, VarIndex>> getAllStmtVarInfo();
	static void performCleanUp();
};

template<class T> unordered_map<VarIndex, unordered_set<StmtIndex, StmtIndex::HashFunction>, VarIndex::HashFunction> RS1<T>::varStmtTable = {};
template<class T> unordered_map<StmtIndex, unordered_set<VarIndex, VarIndex::HashFunction>, StmtIndex::HashFunction> RS1<T>::stmtVarTable = {};
template<class T> unordered_map<VarIndex, unordered_set<ProcIndex, ProcIndex::HashFunction>, VarIndex::HashFunction> RS1<T>::varProcTable = {};
template<class T> unordered_map<ProcIndex, unordered_set<VarIndex, VarIndex::HashFunction>, ProcIndex::HashFunction> RS1<T>::procVarTable = {};

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
unordered_set<StmtIndex, StmtIndex::HashFunction> RS1<T>::getStatements(VarIndex varIndex) {
	return varStmtTable[varIndex];
};

template<class T>
unordered_set<ProcIndex, ProcIndex::HashFunction> RS1<T>::getProcedures(VarIndex varIndex) {
	return varProcTable[varIndex];
};

template<class T>
unordered_set<VarIndex, VarIndex::HashFunction> RS1<T>::getVariables(StmtIndex  stmtIndex) {
	return stmtVarTable[stmtIndex];
};

template<class T>
unordered_set<VarIndex, VarIndex::HashFunction> RS1<T>::getVariables(ProcIndex procIndex) {
	return procVarTable[procIndex];
};

template<class T>
vector<tuple<ProcIndex, VarIndex>> RS1<T>::getAllProcVarInfo() {
	vector<tuple<ProcIndex, VarIndex>> data;
	for (auto& procVarEntry : procVarTable) {
		for (auto& varIndex : procVarEntry.second) {
			data.push_back(make_tuple(procVarEntry.first, varIndex));
		}
	}
	return data;
};

template<class T>
vector<tuple<StmtIndex, VarIndex>> RS1<T>::getAllStmtVarInfo() {
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
