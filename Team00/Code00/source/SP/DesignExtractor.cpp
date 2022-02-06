#include "DesignExtractor.h"

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
	ProcessNode(ast.getRoot());
}
