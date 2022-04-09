#pragma once

#include "../PKB/PKBInserter.h"
#include "CFG.h"
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

	/* Populates entities */
	void insertStmt();
	void insertVar();
	void insertPattern();
	void insertConst();
	void insertProc();
	void insertStmtFromProc();

	/* Constructs and processes CFGs */
	void processCFGs(ProgramNode* programNode);
	void generateCFG(StmtLstNode* stmtLstNode);
	void generateCFGFromStmt(StmtNode* currNode, StmtIndex currStmtIdx, StmtIndex nextStmtIdx);
	void generateCFGfromIfStmt(StmtNode* currNode, StmtIndex currStmtIdx, StmtIndex nextStmtIdx);
	void generateCFGfromWhileStmt(StmtNode* currNode, StmtIndex currStmtIdx, StmtIndex nextStmtIdx);
	void insertNext();

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
