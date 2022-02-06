#pragma once
#include "SourceAST.h"

#include "../PKB/Entity.h"

class DesignExtractor {
private:
	DesignExtractor() {}
	static void processNode(ProgramNode* programNode);
	static void processNode(ProcedureNode* procedureNode);
	static void processNode(StmtListNode* stmtListNode);
	static void processNode(StmtNode* stmtNode);
public:
	static void Extract(SourceAST& ast);
};
