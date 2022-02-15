#pragma once
#include "SourceAST.h"

#include "../PKB/Entity.h"
#include "../PKB/Modifies.h"
#include "../PKB/Uses.h"
#include "../PKB/Pattern.h"
#include "../PKB/Follows.h"
#include "../PKB/FollowsT.h"
#include "../PKB/Parent.h"
#include "../PKB/ParentT.h"

class DesignExtractor {
private:
	/* Maps a ProcIndex to a vector of the StmtIndices contained in it */
	static std::unordered_map<ProcIndex, std::vector<StmtIndex>, ProcIndex::HashFunction> procStmtMap;

	/* Maps a StmtIndex to a vector of the StmtIndices contained in it, if applicable */
	static std::unordered_map<StmtIndex, std::vector<StmtIndex>, StmtIndex::HashFunction> stmtParentMap;

	/* Maps a StmtIndex to the StmtIndex that follows it, if applicable*/
	static std::unordered_map<StmtIndex, StmtIndex, StmtIndex::HashFunction> stmtFollowsMap;

	DesignExtractor() {}
	static void processProgramNode(ProgramNode* programNode);
	static void processProcedureNode(ProcedureNode* procedureNode);
	static std::vector<StmtIndex> processStmtLstNode(StmtLstNode* stmtLstNode);
	static StmtIndex
		processStmtNode(StmtNode* stmtNode, StmtIndex prevIndex);
public:
	static void Extract(SourceAST& ast);
	static std::unordered_map<ProcIndex, std::vector<StmtIndex>, ProcIndex::HashFunction>
		getProcStmtMap();
	static std::unordered_map<StmtIndex, std::vector<StmtIndex>, StmtIndex::HashFunction>
		getStmtParentMap();
	static std::unordered_map<StmtIndex, StmtIndex, StmtIndex::HashFunction>
		getStmtFollowsMap();
};
