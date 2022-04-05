#pragma once

#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../Common/Types.h"
#include "./Attribute.h"
#include "./BidirectionalTable/BidirectionalTableOneWaySet.h"

class Entity {
private:
	static size_t getStmtTypeTableSize();
	static bool isContainerStmt(StmtIndex& stmtIdx);

	static BidirectionalIndexTable<VarIndex> varIdxBidirectionalTable;
	static BidirectionalIndexTable<ProcIndex> procIdxBidirectionalTable;
	static std::unordered_set<ConstValue> constTable;
	static BidirectionalTableOneWaySet<StatementType, StmtIndex> stmtBidirectionalTable;
	static BidirectionalTableOneWaySet<ProcIndex, StmtIndex> procStmtBidirectionalTable;

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
	static StatementType getTypeFromStmtIdx(StmtIndex stmtIdx);
	static std::vector<StmtIndex> getAllStmts();
	static std::vector<StmtIndex> getAllContainerStmts();

	static void insertStmtFromProc(ProcIndex procIdx, StmtIndex stmtIdx);
	static std::vector<StmtIndex> getStmtsFromProc(ProcIndex& procIdx);
	static ProcIndex getProcFromStmt(StmtIndex stmtIdx);
	static std::tuple<std::vector<ProcIndex>, std::vector<StmtIndex>> getAllProcStmts();

	static void performCleanUp();
};
