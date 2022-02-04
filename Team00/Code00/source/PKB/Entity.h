#pragma once

#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <tuple>
#include <vector>

#include "../common/Types.h"

class Entity {
protected:
	static int getVarTableSize();
	static int getProcTableSize();
	static int getStmtTypeTableSize();
	static unordered_map<VarIndex, string, VarIndex::HashFunction> varNameTable;
	static unordered_map<string, VarIndex> varIdxTable;
	static unordered_map<ProcIndex, string, ProcIndex::HashFunction> procNameTable;
	static unordered_map<string, ProcIndex> procIdxTable;
	static unordered_set<int> constTable;
	static unordered_map<StmtIndex, StatementType, StmtIndex::HashFunction> stmtTypeTable;
	static unordered_map<StatementType, unordered_set<StmtIndex, StmtIndex::HashFunction>> stmtIdxFromTypeTable;
	static unordered_map<ProcIndex, unordered_set<StmtIndex, StmtIndex::HashFunction>, ProcIndex::HashFunction> procStmtTable;

public:
	static void insertVar(string varName);
	static string getVarName(VarIndex varIdx);
	static VarIndex getVarIdx(string varName);
	static vector<string> getAllVars();
	static void insertProc(string procName);
	static string getProcName(ProcIndex procIdx);
	static ProcIndex getProcIdx(string procName);
	static vector<string> getAllProcs();
	static void insertConst(int constant);
	static vector<int> getAllConsts();
	static StmtIndex insertStmt(StatementType stmtType);
	static unordered_set<StmtIndex, StmtIndex::HashFunction> getStmtIdxFromType(StatementType stmtType);
	static vector<StmtIndex> getAllStmts();
	static void insertStmtFromProc(ProcIndex procIdx, StmtIndex stmtIdx);
	static unordered_set<StmtIndex, StmtIndex::HashFunction> getStmtsFromProc(ProcIndex procIdx);
	static void performCleanUp();
};

#endif
