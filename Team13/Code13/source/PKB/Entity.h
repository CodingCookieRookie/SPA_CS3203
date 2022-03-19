#pragma once

#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../Common/Types.h"
#include "./Attribute.h"

class Entity {
protected:
	static size_t getVarTableSize();
	static size_t getProcTableSize();
	static size_t getStmtTypeTableSize();
	static bool isContainerStmt(StmtIndex& stmtIdx);

	static std::unordered_map<VarIndex, std::string> varNameTable;
	static std::unordered_map<ProcIndex, std::string> procNameTable;
	static std::unordered_set<ConstValue> constTable;
	static std::unordered_map<StmtIndex, StatementType> stmtTypeTable;
	static std::unordered_map<StatementType, std::unordered_set<StmtIndex>> stmtIdxFromTypeTable;
	static std::unordered_map<ProcIndex, std::unordered_set<StmtIndex>> procStmtTable;

public:
	static VarIndex insertVar(std::string varName);
	static bool containsVar(std::string& varName);
	static std::string getVarName(VarIndex varIdx);
	static VarIndex getVarIdx(std::string varName);
	static std::vector<VarIndex> getAllVars();

	static ProcIndex insertProc(std::string procName);
	static bool containsProc(std::string& procName);
	static std::string getProcName(ProcIndex procIdx);
	static ProcIndex getProcIdx(std::string procName);
	static std::vector<ProcIndex> getAllProcs();

	static void insertConst(ConstValue constant);
	static std::vector<ConstValue> getAllConsts();

	static StmtIndex insertStmt(StatementType stmtType);
	static StmtIndex insertStmt(StatementType stmtType, std::string& nameValue);
	static bool containsStmt(StmtIndex stmtNo);
	static std::vector<StmtIndex> getStmtIdxFromType(StatementType stmtType);
	static std::vector<StmtIndex> getAllStmts();
	static std::vector<StmtIndex> getAllContainerStmts();

	static void insertStmtFromProc(ProcIndex procIdx, StmtIndex stmtIdx);
	static std::unordered_set<StmtIndex> getStmtsFromProc(ProcIndex& procIdx);
	static std::unordered_map<ProcIndex, std::unordered_set<StmtIndex>> getAllProcStmts();

	static void performCleanUp();
};
