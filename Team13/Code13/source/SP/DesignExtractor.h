#pragma once

#include "../PKB/TransitivePopulator.h"
#include "../PKB/Pattern.h"
#include "CFG.h"
#include "CFGNode.h"
#include "SourceAST.h"

class DesignExtractor {
private:
	/* Maps a StmtIndex to a vector of the StmtIndices contained in it, if applicable */
	static std::unordered_map<StmtIndex, std::vector<StmtIndex>> stmtParentMap;

	/* Maps a StmtIndex to the StmtIndex that follows it, if applicable */
	static std::unordered_map<StmtIndex, StmtIndex> stmtFollowsMap;

	/* Maps a StmtNode to the StmtIndex of the statement */
	static std::unordered_map<StmtNode*, StmtIndex> DesignExtractor::stmtNodeIndexMap;

	DesignExtractor() {}
	static void processProgramNode(ProgramNode* programNode);
	static void processProcedureNode(ProcedureNode* procedureNode);
	static std::vector<StmtIndex> processStmtLstNode(StmtLstNode* stmtLstNode);
	static StmtIndex processStmtNode(StmtNode* stmtNode, StmtIndex prevIndex);
	static StmtIndex insertStmt(StmtNode* stmtNode);
	static void appendNode(CFGNode*& head, CFGNode*& tail, CFGNode* next);
	static void processCFGStmtIndices(std::unordered_set<StmtIndex>& stmtIndices, CFGNode*& head, CFGNode*& tail);

public:
	static void extract(SourceAST& ast);
	static CFG generateCFG(StmtLstNode* stmtLstNode);
	static std::unordered_map<StmtIndex, std::vector<StmtIndex>> getStmtParentMap();
	static std::unordered_map<StmtIndex, StmtIndex> getStmtFollowsMap();
};
