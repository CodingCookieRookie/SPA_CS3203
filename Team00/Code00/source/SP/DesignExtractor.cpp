#include "DesignExtractor.h"

void DesignExtractor::processProgramNode(ProgramNode* programNode) {
	for (ProcedureNode* procedureNode : programNode->getProcedureNodes()) {
		processProcedureNode(procedureNode);
	}
}

void DesignExtractor::processProcedureNode(ProcedureNode* procedureNode) {
	Entity::insertProc(procedureNode->getProcName());
	processStmtLstNode(procedureNode->getStmtListNode());
}

void DesignExtractor::processStmtLstNode(StmtListNode* stmtListNode) {
	for (StmtNode* stmtNode : stmtListNode->getStmtNodes()) {
		processStmtNode(stmtNode);
	}
}

void DesignExtractor::processStmtNode(StmtNode* stmtNode) {
	Entity::insertStmt(stmtNode->getStmtType());
	std::vector<std::string> modifies = stmtNode->getModifies();
	for (std::string& varName : modifies) {
		Entity::insertVar(varName);
	}
	std::vector<std::string> uses = stmtNode->getUses();
	for (std::string& varName : uses) {
		Entity::insertVar(varName);
	}
}

void DesignExtractor::Extract(SourceAST& ast) {
	processProgramNode(ast.getRoot());
}
