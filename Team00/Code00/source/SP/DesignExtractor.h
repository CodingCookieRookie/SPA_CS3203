#pragma once
#include "SourceAST.h"

#include "../PKB/Entity.h"

class DesignExtractor {
private:
	DesignExtractor() {}
	static void processProgramNode(ProgramNode* programNode);
	static void processProcedureNode(ProcedureNode* procedureNode);
	static void processStmtLstNode(StmtLstNode* stmtLstNode);
	static void processStmtNode(StmtNode* stmtNode);
public:
	static void Extract(SourceAST& ast);
};
