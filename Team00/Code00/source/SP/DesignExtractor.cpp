#include "DesignExtractor.h"

void DesignExtractor::processProgramNode(ProgramNode* programNode) {
	for (ProcedureNode* procedureNode : programNode->getProcedureNodes()) {
		processProcedureNode(procedureNode);
	}
}

void DesignExtractor::processProcedureNode(ProcedureNode* procedureNode) {
	ProcIndex procIdx = Entity::insertProc(procedureNode->getProcName());
	processStmtLstNode(procedureNode->getStmtLstNode());
}

void DesignExtractor::processStmtLstNode(StmtLstNode* stmtLstNode) {
	for (StmtNode* stmtNode : stmtLstNode->getStmtNodes()) {
		processStmtNode(stmtNode);
	}
}

void DesignExtractor::processStmtNode(StmtNode* stmtNode) {
	Entity::insertStmt(stmtNode->getStmtType());
	std::unordered_set<std::string> modifies = stmtNode->getModifies();
	for (const std::string& varName : modifies) {
		Entity::insertVar(varName);
	}
	std::unordered_set<std::string> uses = stmtNode->getUses();
	for (const std::string& varName : uses) {
		Entity::insertVar(varName);
	}
}

void DesignExtractor::Extract(SourceAST& ast) {
	processProgramNode(ast.getRoot());
}
