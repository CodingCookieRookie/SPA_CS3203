#pragma once

#include "../PKB/Next.h"
#include "../PKB/TransitivePopulator.h"
#include "../PKB/Pattern.h"
#include "../PKB/CFG.h"
#include "SourceAST.h"

class DesignExtractor {
private:
	DesignExtractor() {}
	static void processProgramNode(
		ProgramNode* programNode,
		std::unordered_map<ProcIndex, std::vector<StmtIndex>>& procStmtMap,
		std::unordered_map<StmtIndex, std::string>& callsProcNameMap,
		std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap);
	static void processProcedureNode(
		ProcedureNode* procedureNode,
		std::unordered_map<ProcIndex, std::vector<StmtIndex>>& procStmtMap,
		std::unordered_map<StmtIndex, std::string>& callsProcNameMap,
		std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap);
	static std::vector<StmtIndex> processStmtLstNode(
		StmtLstNode* stmtLstNode,
		std::unordered_map<StmtIndex, std::string>& callsProcNameMap,
		std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap);
	static StmtIndex processStmtNode(
		StmtNode* stmtNode,
		StmtIndex& prevIndex,
		std::unordered_map<StmtIndex, std::string>& callsProcNameMap,
		std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap);

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
		std::unordered_map<StmtIndex, std::string>& callsProcNameMap,
		std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap);
	static void insertNext(CFG* cfg);

	static void processProcStmtMap(std::unordered_map<ProcIndex, std::vector<StmtIndex>>& procStmtMap);
	static void insertStmtFromProc(ProcIndex& procIndex, StmtIndex& stmtIndex);

	static void processCallsProcNameMap(std::unordered_map<StmtIndex, std::string>& callsProcNameMap);

	/* Constructs and processes CFGs */
	static void processCFGs(
		ProgramNode* programNode,
		CFG* cfg,
		std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap);
	static void generateCFG(
		StmtLstNode* stmtLstNode,
		CFG* cfg,
		std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap);
	static void generateCFGFromStmt(
		StmtNode* currNode,
		CFG* cfg,
		std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap,
		int currStmtIdx,
		int nextStmtIdx);

public:
	static void extract(SourceAST& ast, CFG* cfg);
};
