#pragma once

#include "../PKB/PKBInserter.h"
#include "../SP/CFG.h"
#include "SourceAST.h"

class DesignExtractor {
private:
	static std::unordered_map<RelationshipType, RelationshipType> rsStoPMap;
	static std::unordered_map<RelationshipType, RelationshipType> rsPtoSMap;
	static std::unordered_map<RelationshipType, RelationshipType> rsToRsTMap;

	SourceAST ast;
	PKBInserter* pkbInserter;
	CFG* cfg;

	/* Maps RelationshipType to its corresponding maps populated by the AST when it's processed */
	std::unordered_map<RelationshipType, RelationshipMap> rsToASTMap;

	/* Maps RelationshipType to its corresponding populated indices */
	std::unordered_map<RelationshipType, std::unordered_set<SynonymIndex>> rsToPopulatedIndicesMap;

	/* TODO: 1) remove dead code, 2) remove ast and pkbInserter from params */

	/* Populates relationships */
	void insertNext(PKBInserter* pkbInserter);

	/* Populates entities */
	void insertStmt(SourceAST& ast, std::unordered_map<StmtNode*, StmtIndex>& stmtNodeIndexMap);
	void insertVar(SourceAST& ast);
	void insertPattern(SourceAST& ast);
	void insertConst(SourceAST& ast);
	void insertProc(SourceAST& ast);
	void insertStmtFromProc(SourceAST& ast);

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

	/* PKB population for diff synonyms RS (UsesS/P, ModifiesS/P) */
	void populateDiffSynonymsRSInfo();
	void populateDiffSynonymsRSStmtInfo(RelationshipType rsTypeS, StmtIndex stmtIndex);
	void populateDiffSynonymsRSProcInfo(RelationshipType rsTypeP, ProcIndex procIndex);

	/* PKB population for same synonyms RS (Follows/T, Parent/T, Calls/T) */
	void populateSameSynonymsRSInfo();
	std::unordered_set<SynonymIndex> getAllSuccessors(SynonymIndex predecessor, RelationshipMap& rsMap, RelationshipMap& rsTMap);

public:
	DesignExtractor(SourceAST& ast, PKBInserter* pkbInserter);
	~DesignExtractor();
	void extract();
	std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> getCFG();
};
