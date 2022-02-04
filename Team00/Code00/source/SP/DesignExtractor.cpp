#include "DesignExtractor.h"
#include "../PKB/Entity.h"

void DesignExtractor::ProcessNode(ProgramNode* programNode) {
	for (ProcedureNode* procedureNode : programNode->getProcedureNodes()) {
		ProcessNode(procedureNode);
	}
}

void DesignExtractor::ProcessNode(ProcedureNode* procedureNode) {
	Entity::insertProc(procedureNode->getProcName());
	ProcessNode(procedureNode->getStmtListNode());
}

void DesignExtractor::ProcessNode(StmtListNode* stmtListNode) {
	for (StmtNode* stmtNode : stmtListNode->getStmtNodes()) {
		ProcessNode(stmtNode);
	}
}

void DesignExtractor::ProcessNode(StmtNode* stmtNode) {
	// We should never reach this, because we
	// should never instanciate a StmtNode that
	// isn't one of the subtypes
}

void DesignExtractor::ProcessNode(ReadNode* readNode) {
	Entity::insertStmt(StatementType::readType);
	Entity::insertVar(readNode->getVarName());
}

void DesignExtractor::ProcessNode(PrintNode* printNode) {
	Entity::insertStmt(StatementType::printType);
	Entity::insertVar(printNode->getVarName());
}

void DesignExtractor::Extract(SourceAST& ast) {
	ProcessNode(ast.getRoot());
}
