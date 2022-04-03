#pragma once

#include "../PKB/Next.h"
#include "../PKB/TransitivePopulator.h"
#include "../PKB/Pattern.h"
#include "../PKB/CFG.h"
#include "../PKB/PKBInserter.h"
#include "SourceAST.h"

class DesignExtractor {
private:
	DesignExtractor() {}

	/* Populates relationships */
	static void insertFollows(SourceAST& ast);
	static void insertModifies(SourceAST& ast);
	static void insertUses(SourceAST& ast);
	static void insertParent(SourceAST& ast);
	static void insertCalls(SourceAST& ast);
	static void insertNext(PKBInserter* pkb);

	/* Populates entities */
	static void insertStmt(SourceAST& ast, std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap);
	static void insertPattern(SourceAST& ast);
	static void insertConst(SourceAST& ast);
	static void insertProc(SourceAST& ast);
	static void insertStmtFromProc(SourceAST& ast);

	/* Constructs and processes CFGs */
	static void processCFGs(
		ProgramNode* programNode,
		PKBInserter* pkb,
		std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap);
	static void generateCFG(
		StmtLstNode* stmtLstNode,
		PKBInserter* pkb,
		std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap);
	static void generateCFGFromStmt(
		StmtNode* currNode,
		PKBInserter* pkb,
		std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap,
		int currStmtIdx,
		int nextStmtIdx);

public:
	static void extract(SourceAST& ast, PKBInserter* pkb);
};
