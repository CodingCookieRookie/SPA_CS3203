#pragma once
#include "SourceAST.h"

#include "../PKB/Entity.h"

class DesignExtractor {
private:
	static std::unordered_map<ProcIndex, std::vector<StmtIndex>, ProcIndex::HashFunction> procStmtMap;
	static std::unordered_map<StmtIndex, std::vector<StmtIndex>, StmtIndex::HashFunction> stmtParentMap;

	DesignExtractor() {}
	static void processProgramNode(ProgramNode* programNode);
	static void processProcedureNode(ProcedureNode* procedureNode);
	static std::vector<StmtIndex> processStmtLstNode(StmtLstNode* stmtLstNode);
	static StmtIndex processStmtNode(StmtNode* stmtNode);
public:
	static void Extract(SourceAST& ast);
	static std::unordered_map<ProcIndex, std::vector<StmtIndex>, ProcIndex::HashFunction>
		getProcStmtMap();
	static std::unordered_map<StmtIndex, std::vector<StmtIndex>, StmtIndex::HashFunction>
		getStmtParentMap();
};
