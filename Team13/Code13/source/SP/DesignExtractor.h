#pragma once

#include "../PKB/Next.h"
#include "../PKB/TransitivePopulator.h"
#include "../PKB/Pattern.h"
#include "../PKB/PKBCFG.h"
#include "CFG.h"
#include "SourceAST.h"

class DesignExtractor {
private:
	/* Maps a StmtNode to the StmtIndex of the statement */
	static std::unordered_map<StmtNode*, StmtIndex> DesignExtractor::stmtNodeIndexMap;

	DesignExtractor() {}
	static void processProgramNode(
		ProgramNode* programNode,
		std::unordered_map<ProcIndex, std::vector<StmtIndex>>& procStmtMap,
		std::unordered_map<StmtIndex, std::string>& callsProcNameMap);
	static void processProcedureNode(
		ProcedureNode* procedureNode,
		std::unordered_map<ProcIndex, std::vector<StmtIndex>>& procStmtMap,
		std::unordered_map<StmtIndex, std::string>& callsProcNameMap);
	static std::vector<StmtIndex> processStmtLstNode(
		StmtLstNode* stmtLstNode,
		std::unordered_map<StmtIndex, std::string>& callsProcNameMap);
	static StmtIndex processStmtNode(
		StmtNode* stmtNode,
		StmtIndex& prevIndex,
		std::unordered_map<StmtIndex, std::string>& callsProcNameMap);

	/* Populates the relevant Entity and Relationship */
	static StmtIndex insertStmt(StmtNode* stmtNode);
	static void insertConst(StmtNode* stmtNode);
	static void insertPattern(StmtNode* stmtNode, StmtIndex& stmtIndex);
	static void insertFollows(StmtIndex& prevIndex, StmtIndex& stmtIndex);
	static void insertModifies(StmtNode* stmtNode, StmtIndex& stmtIndex);
	static void insertUses(StmtNode* stmtNode, StmtIndex& stmtIndex);
	static void insertParent(
		StmtNode* stmtNode,
		StmtIndex& stmtIndex,
		std::unordered_map<StmtIndex, std::string>& callsProcNameMap);

	static void processProcStmtMap(std::unordered_map<ProcIndex, std::vector<StmtIndex>>& procStmtMap);
	static void insertStmtFromProc(ProcIndex& procIndex, StmtIndex& stmtIndex);

	static void processCallsProcNameMap(std::unordered_map<StmtIndex, std::string>& callsProcNameMap);

	/* CFG construction */
	static void appendNode(CFGNode*& head, CFGNode*& tail, CFGNode* next);
	static void processCFGStmtIndices(std::set<StmtIndex>& stmtIndices, CFGNode*& head, CFGNode*& tail);
	static void processCFGs(ProgramNode* programNode);
	static void extractNextInfo(CFGNode* node);

public:
	static void extract(SourceAST& ast);
	static CFG generateCFG(StmtLstNode* stmtLstNode);
};
