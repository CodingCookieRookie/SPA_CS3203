#pragma once

#include "../Exception/SPAException.h"
#include "../PKB/PKB.h"
#include "../PKB/Pattern.h"
#include "SourceAST.h"

class DesignExtractor {
private:
	/* Maps a ProcIndex to a vector of the StmtIndices contained in it */
	static std::unordered_map<ProcIndex, std::vector<StmtIndex>> procStmtMap;

	/* Maps a StmtIndex to a vector of the StmtIndices contained in it, if applicable */
	static std::unordered_map<StmtIndex, std::vector<StmtIndex>> stmtParentMap;

	/* Maps a StmtIndex to the StmtIndex that follows it, if applicable */
	static std::unordered_map<StmtIndex, StmtIndex> stmtFollowsMap;

	/* Maps a ProcIndex to the set of ProcIndex of the procedures that the former calls */
	static std::unordered_map<ProcIndex, std::unordered_set<ProcIndex>> callsMap;

	DesignExtractor() {}
	static void validateAST(ProgramNode* progNode);
	static void validateNoDuplicateProc(ProgramNode* progNode);
	static void populateCalls(ProgramNode* progNode);
	static void processCallsStmtLstNode(StmtLstNode* stmtLstNode, std::unordered_set<ProcIndex>& calledProcs);
	static void processCallsStmtNode(StmtNode* stmtNode, std::unordered_set<ProcIndex>& calledProcs);
	static void processProgramNode(ProgramNode* programNode);
	static void processProcedureNode(ProcedureNode* procedureNode);
	static std::vector<StmtIndex> processStmtLstNode(StmtLstNode* stmtLstNode);
	static StmtIndex
		processStmtNode(StmtNode* stmtNode, StmtIndex prevIndex);
public:
	static void extract(SourceAST& ast);
	static std::unordered_map<ProcIndex, std::vector<StmtIndex>> getProcStmtMap();
	static std::unordered_map<StmtIndex, std::vector<StmtIndex>> getStmtParentMap();
	static std::unordered_map<StmtIndex, StmtIndex> getStmtFollowsMap();
};
