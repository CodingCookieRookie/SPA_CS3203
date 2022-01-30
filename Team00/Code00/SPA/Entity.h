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
	//unordered_map<PROC_IDX, string> procNameTable;
	//unordered_map<string, PROC_IDX> procIdxTable;
	//unordered_map<VAR_IDX, string> varNameTable;
	//unordered_map<string, VAR_IDX> varIdxTable;
	//unordered_map<STMT_IDX, string> stmtTypeTable;
	//unordered_map<string, unordered_set<STMT_IDX, STMT_IDX::Hasher>> stmtTypeIdxTable;

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
	//void insertStmtFromProc(PROC_IDX procIdx, STMT_IDX stmtIdx);
	//unordered_set<STMT_IDX, STMT_IDX::Hasher> getStmtsFromProc(PROC_IDX procIdx);

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