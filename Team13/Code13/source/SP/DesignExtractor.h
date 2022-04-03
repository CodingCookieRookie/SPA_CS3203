#pragma once

#include "../PKB/Next.h"
#include "../PKB/TransitivePopulator.h"
#include "../PKB/Pattern.h"
#include "../PKB/PKBInserter.h"
#include "../SP/CFG.h"
#include "SourceAST.h"

class DesignExtractor {
private:
	CFG* cfg;

	/* Populates relationships */
	void insertFollows(SourceAST& ast);
	void insertModifies(SourceAST& ast);
	void insertUses(SourceAST& ast);
	void insertParent(SourceAST& ast);
	void insertCalls(SourceAST& ast);
	void insertNext(PKBInserter* pkb);

	/* Populates entities */
	static void insertStmt(SourceAST& ast, std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap);
	static void insertPattern(SourceAST& ast);
	static void insertConst(SourceAST& ast);
	static void insertProc(SourceAST& ast);
	static void insertStmtFromProc(SourceAST& ast);

	/* Constructs and processes CFGs */
	void processCFGs(
		ProgramNode* programNode,
		PKBInserter* pkb,
		std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap);
	void generateCFG(
		StmtLstNode* stmtLstNode,
		PKBInserter* pkb,
		std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap);
	void generateCFGFromStmt(
		StmtNode* currNode,
		PKBInserter* pkb,
		std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap,
		int currStmtIdx,
		int nextStmtIdx);

public:
	DesignExtractor();
	~DesignExtractor();
	void extract(SourceAST& ast, PKBInserter* pkb);
	std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> getCFG();
};
