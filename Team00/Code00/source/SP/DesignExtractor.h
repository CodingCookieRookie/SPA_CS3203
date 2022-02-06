#pragma once
#include "SourceAST.h"

#include "../PKB/Entity.h"

class DesignExtractor {
private:
	DesignExtractor() {}
	static void ProcessNode(ProgramNode* programNode);
	static void ProcessNode(ProcedureNode* procedureNode);
	static void ProcessNode(StmtListNode* stmtListNode);
	static void ProcessNode(StmtNode* stmtNode);
	static void ProcessNode(ReadNode* readNode);
	static void ProcessNode(PrintNode* printNode);
public:
	static void Extract(SourceAST& ast);
};
