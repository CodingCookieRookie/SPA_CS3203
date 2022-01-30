#pragma once

#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <tuple>
#include <vector>

#include "./Types.h"

class Entity {

protected:
	//static int getVarTableSize();
	//static int getProcTableSize();
	//static int getStmtTypeTableSize();
	//static unordered_map<VAR_IDX, string, VAR_IDX::hash_fn> varNameTable;
	//static unordered_map<string, VAR_IDX, VAR_IDX::hash_fn> varIdxTable;
	//static unordered_map<PROC_IDX, string, PROC_IDX::hash_fn> procNameTable;
	//static unordered_map<string, PROC_IDX, PROC_IDX::hash_fn> procIdxTable;
	//static unordered_set<int> constTable;
	//static unordered_map<STMT_IDX, string, STMT_IDX::hash_fn> stmtTypeTable;
	//static unordered_map<string, unordered_set<STMT_IDX, STMT_IDX::hash_fn>> stmtIdxFromTypeTable;
	//static unordered_map<PROC_IDX, unordered_set<STMT_IDX, STMT_IDX::hash_fn>, PROC_IDX::hash_fn> procStmtTable;

	static int getVarTableSize();
	static int getProcTableSize();
	static int getStmtTypeTableSize();
	static unordered_map<int, string> varNameTable;
	static unordered_map<string, int> varIdxTable;
	static unordered_map<int, string> procNameTable;
	static unordered_map<string, int> procIdxTable;
	static unordered_set<int> constTable;
	static unordered_map<int, string> stmtTypeTable;
	static unordered_map<string, unordered_set<int>> stmtIdxFromTypeTable;
	static unordered_map<int, unordered_set<int>> procStmtTable;

public:
	static int insertNow(VAR_IDX v);
	static int insertNow(PROC_IDX v);

	//static void insertVar(string varName);
	//static string getVarName(VAR_IDX varIdx);
	//static VAR_IDX getVarIdx(string varName);
	//static vector<string> getAllVars();
	//static void insertProc(string procName);
	//static string getProcName(PROC_IDX procIdx);
	//static PROC_IDX getProcIdx(string procName);
	//static vector<string> getAllProcs();
	//static void insertConst(int constant);
	//static vector<int> getAllConsts();
	//static void insertStmt(string stmtType);
	//static unordered_set<STMT_IDX, STMT_IDX::hash_fn> getStmtIdxFromType(string stmtType);
	//static vector<STMT_IDX, STMT_IDX::hash_fn> getAllStmts();
	//static void insertStmtFromProc(PROC_IDX procIdx, STMT_IDX stmtIdx);
	//static unordered_set<STMT_IDX, STMT_IDX::hash_fn> getStmtsFromProc(PROC_IDX procIdx);

	static void insertVar(string varName);
	static string getVarName(int varIdx);
	static int getVarIdx(string varName);
	static vector<string> getAllVars();
	static void insertProc(string procName);
	static string getProcName(int procIdx);
	static int getProcIdx(string procName);
	static vector<string> getAllProcs();
	static void insertConst(int constant);
	static vector<int> getAllConsts();
	static void insertStmt(string stmtType);
	static unordered_set<int> getStmtIdxFromType(string stmtType);
	static vector<int> getAllStmts();
	static void insertStmtFromProc(int procIdx, int stmtIdx);
	static unordered_set<int> getStmtsFromProc(int procIdx);
};

#endif