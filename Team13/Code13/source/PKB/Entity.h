#pragma once

#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../Common/Types.h"

class Entity {
protected:
	static int getVarTableSize();
	static int getProcTableSize();
	static int getStmtTypeTableSize();
	static std::unordered_map<VarIndex, std::string, VarIndex::HashFunction> varNameTable;
	static std::unordered_map<std::string, VarIndex> varIdxTable;
	static std::unordered_map<ProcIndex, std::string, ProcIndex::HashFunction> procNameTable;
	static std::unordered_map<std::string, ProcIndex> procIdxTable;
	static std::unordered_set<int> constTable;
	static std::unordered_map<StmtIndex, StatementType, StmtIndex::HashFunction> stmtTypeTable;
	static std::unordered_map<StatementType, std::unordered_set<StmtIndex, StmtIndex::HashFunction>> stmtIdxFromTypeTable;
	static std::unordered_map<ProcIndex, std::unordered_set<StmtIndex, StmtIndex::HashFunction>, ProcIndex::HashFunction> procStmtTable;

public:
	static VarIndex insertVar(std::string varName);
	static bool containsVar(std::string varName);
	static std::string getVarName(VarIndex varIdx);
	static VarIndex getVarIdx(std::string varName);
	static std::vector<VarIndex> getAllVars();
	static ProcIndex insertProc(std::string procName);
	static bool containsProc(std::string procName);
	static std::string getProcName(ProcIndex procIdx);
	static ProcIndex getProcIdx(std::string procName);
	static std::vector<ProcIndex> getAllProcs();
	static void insertConst(int constant);
	static std::vector<int> getAllConsts();
	static StmtIndex insertStmt(StatementType stmtType);
	static bool isContainerStmt(StmtIndex stmtIdx);
	static bool containsStmt(int stmtNo);
	static std::vector<StmtIndex> getStmtIdxFromType(StatementType stmtType);
	static std::vector<StmtIndex> getAllStmts();
	static void insertStmtFromProc(ProcIndex procIdx, StmtIndex stmtIdx);
	static std::unordered_set<StmtIndex, StmtIndex::HashFunction> getStmtsFromProc(ProcIndex procIdx);
	static void performCleanUp();
};
