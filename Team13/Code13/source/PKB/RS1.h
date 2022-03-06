#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../Common/Types.h"

template<class T>
class RS1 {
protected:
	RS1();
	static std::unordered_map<VarIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>,
		VarIndex::HashFunction> varStmtTable;
	static std::unordered_map<StmtIndex, std::unordered_set<VarIndex, VarIndex::HashFunction>,
		StmtIndex::HashFunction> stmtVarTable;
	static std::unordered_map<VarIndex, std::unordered_set<ProcIndex, ProcIndex::HashFunction>,
		VarIndex::HashFunction> varProcTable;
	static std::unordered_map<ProcIndex, std::unordered_set<VarIndex, VarIndex::HashFunction>,
		ProcIndex::HashFunction> procVarTable;

public:
	static void insert(StmtIndex stmtIndex, VarIndex varIndex);
	static void insert(ProcIndex procIndex, VarIndex varIndex);
	static bool contains(StmtIndex& stmtIndex, VarIndex& varIndex);
	static bool contains(ProcIndex& procIndex, VarIndex& varIndex);
	static std::vector<int> getStatements(VarIndex& varIndex);
	static std::vector<int> getProcedures(VarIndex& varIndex);
	static std::vector<int> getVariables(StmtIndex& stmtIndex);
	static std::vector<int> getVariables(ProcIndex& procIndex);
	static std::tuple<std::vector<int>, std::vector<int>> getAllProcVarInfo();
	static std::tuple<std::vector<int>, std::vector<int>> getAllStmtVarInfo();
	static void populateForContainers(StmtIndex& containerStmt,
		std::unordered_set<StmtIndex, StmtIndex::HashFunction>& subStmts);
	static void performCleanUp();
};

template<class T> std::unordered_map<VarIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>,
	VarIndex::HashFunction> RS1<T>::varStmtTable = {};
template<class T> std::unordered_map<StmtIndex, std::unordered_set<VarIndex, VarIndex::HashFunction>,
	StmtIndex::HashFunction> RS1<T>::stmtVarTable = {};
template<class T> std::unordered_map<VarIndex, std::unordered_set<ProcIndex, ProcIndex::HashFunction>,
	VarIndex::HashFunction> RS1<T>::varProcTable = {};
template<class T> std::unordered_map<ProcIndex, std::unordered_set<VarIndex, VarIndex::HashFunction>,
	ProcIndex::HashFunction> RS1<T>::procVarTable = {};

template<class T>
RS1<T>::RS1() {};

template<class T>
void RS1<T>::insert(StmtIndex stmtIndex, VarIndex varIndex) {
	varStmtTable[varIndex].insert(stmtIndex);
	stmtVarTable[stmtIndex].insert(varIndex);
};

template<class T>
void RS1<T>::insert(ProcIndex procIndex, VarIndex varIndex) {
	varProcTable[varIndex].insert(procIndex);
	procVarTable[procIndex].insert(varIndex);
};

template<class T>
bool RS1<T>::contains(StmtIndex& stmtIndex, VarIndex& varIndex) {
	if (stmtVarTable.find(stmtIndex) == stmtVarTable.end()) {
		return false;
	}

	std::unordered_set<VarIndex, VarIndex::HashFunction> variables = stmtVarTable[stmtIndex];
	if (variables.find(varIndex) == variables.end()) {
		return false;
	}

	return true;
};

template<class T>
bool RS1<T>::contains(ProcIndex& procIndex, VarIndex& varIndex) {
	if (procVarTable.find(procIndex) == procVarTable.end()) {
		return false;
	}

	std::unordered_set<VarIndex, VarIndex::HashFunction> variables = procVarTable[procIndex];
	if (variables.find(varIndex) == variables.end()) {
		return false;
	}

	return true;
};

template<class T>
std::vector<int> RS1<T>::getStatements(VarIndex& varIndex) {
	std::vector<int> statements;
	for (auto& statement : varStmtTable[varIndex]) {
		statements.push_back(statement.index);
	}
	return statements;
};

template<class T>
std::vector<int> RS1<T>::getProcedures(VarIndex& varIndex) {
	std::vector<int> procedures;
	for (auto& procedure : varProcTable[varIndex]) {
		procedures.push_back(procedure.index);
	}
	return procedures;
};

template<class T>
std::vector<int> RS1<T>::getVariables(StmtIndex& stmtIndex) {
	std::vector<int> variables;
	for (auto& variable : stmtVarTable[stmtIndex]) {
		variables.push_back(variable.index);
	}
	return variables;
};

template<class T>
std::vector<int> RS1<T>::getVariables(ProcIndex& procIndex) {
	std::vector<int> variables;
	for (auto& variable : procVarTable[procIndex]) {
		variables.push_back(variable.index);
	}
	return variables;
};

template<class T>
std::tuple<std::vector<int>, std::vector<int>> RS1<T>::getAllProcVarInfo() {
	std::vector<int> procedures;
	std::vector<int> variables;
	for (auto& procVarEntry : procVarTable) {
		for (auto& varIndex : procVarEntry.second) {
			procedures.push_back(procVarEntry.first.index);
			variables.push_back(varIndex.index);
		}
	}
	return std::make_tuple(procedures, variables);
};

template<class T>
std::tuple<std::vector<int>, std::vector<int>> RS1<T>::getAllStmtVarInfo() {
	std::vector<int> statements;
	std::vector<int> variables;
	for (auto& stmtVarEntry : stmtVarTable) {
		for (auto& varIndex : stmtVarEntry.second) {
			statements.push_back(stmtVarEntry.first.index);
			variables.push_back(varIndex.index);
		}
	}
	return std::make_tuple(statements, variables);
};

template<class T>
void RS1<T>::populateForContainers(StmtIndex& containerStmt, std::unordered_set<StmtIndex, StmtIndex::HashFunction>& subStmts) {
	for (auto& stmt : subStmts) {
		for (auto& var : stmtVarTable[stmt]) {
			insert(containerStmt, var);
		}
	}
}

template<class T>
void RS1<T>::performCleanUp() {
	varStmtTable = {};
	stmtVarTable = {};
	varProcTable = {};
	procVarTable = {};
}
