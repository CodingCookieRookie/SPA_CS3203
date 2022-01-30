#pragma once

#ifndef ENTITY_H
#define ENTITY_H

#include "./Types.h"

class Entity {

protected:
	//unordered_map<PROC_IDX, string> procNameTable;
	//unordered_map<string, PROC_IDX> procIdxTable;
	//unordered_map<VAR_IDX, string> varNameTable;
	//unordered_map<string, VAR_IDX> varIdxTable;
	//unordered_map<STMT_IDX, string> stmtTypeTable;
	//unordered_map<string, unordered_set<STMT_IDX, STMT_IDX::Hasher>> stmtTypeIdxTable;

	unordered_map<int, string> procNameTable;
	unordered_map<string, int> procIdxTable;
	unordered_map<int, string> varNameTable;
	unordered_map<string, int> varIdxTable;
	unordered_set<int> constTable;
	unordered_map<int, string> stmtTypeTable;
	unordered_map<string, unordered_set<int>> stmtIdxFromTypeTable;
	unordered_map<int, unordered_set<int>> procStmtTable;

public:
	static int insertNow(VAR_IDX v);
	static int insertNow(PROC_IDX v);
	static int* getInstance();

	//void insertVar(string varName);
	//string getVarName(VAR_IDX varIdx);
	//VAR_IDX getVarIdx(string varName);
	//unordered_set<VAR_IDX, VAR_IDX::Hasher> getAllVars();
	//void insertProc(string procName);
	//string getProcName(PROC_IDX procIdx);
	//PROC_IDX getProcIdx(string procName);
	//unordered_set<PROC_IDX, PROC_IDX::Hasher> getAllProcs();
	//void insertConst(int const); 
	//unordered_set<int> getAllConsts();
	//void insertStmt(string stmtType);
	//unordered_set<STMT_IDX, STMT_IDX::Hasher> getStmtIdxFromType(string stmtType);
	//unordered_set<PROC_IDX, PROC_IDX::Hasher> getAllStmts();
	//unordered_set<STMT_IDX, STMT_IDX::Hasher> getStmtsFromProc(PROC_IDX procIdx);

	void insertVar(string varName);
	string getVarName(int varIdx);
	int getVarIdx(string varName);
	unordered_set<int> getAllVars();
	void insertProc(string procName);
	string getProcName(int procIdx);
	int getProcIdx(string procName);
	unordered_set<int> getAllProcs();
	void insertConst(int const); 
	unordered_set<int> getAllConsts();
	void insertStmt(string stmtType);
	unordered_set<int> getStmtIdxFromType(string stmtType);
	unordered_set<int> getAllStmts();
	unordered_set<int> getStmtsFromProc(int procIdx);
};

#endif