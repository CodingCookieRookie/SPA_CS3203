#include "DesignExtractor.h"

void DesignExtractor::processNode(ProgramNode* programNode) {
	for (ProcedureNode* procedureNode : programNode->getProcedureNodes()) {
		processNode(procedureNode);
	}
}

void DesignExtractor::processNode(ProcedureNode* procedureNode) {
	Entity::insertProc(procedureNode->getProcName());
	processNode(procedureNode->getStmtListNode());
}

void DesignExtractor::processNode(StmtListNode* stmtListNode) {
	for (StmtNode* stmtNode : stmtListNode->getStmtNodes()) {
		processNode(stmtNode);
	}
}

void DesignExtractor::processNode(StmtNode* stmtNode) {
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
	processNode(ast.getRoot());
}
