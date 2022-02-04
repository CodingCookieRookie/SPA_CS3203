#include "DesignExtractor.h"
//#include "PKB.h"

void DesignExtractor::ProcessNode(ProgramNode* programNode) {
	for (ProcedureNode* procedureNode : programNode->getProcedureNodes()) {
		ProcessNode(procedureNode);
	}
}

void DesignExtractor::ProcessNode(ProcedureNode* procedureNode) {
	ProcessNode(procedureNode->getStmtListNode());
}

void DesignExtractor::ProcessNode(StmtListNode* stmtListNode) {
	for (StmtNode* stmtNode : stmtListNode->getStmtNodes()) {
		ProcessNode(stmtNode);
	}
}

void DesignExtractor::ProcessNode(ReadNode* readNode) {

}

void DesignExtractor::Extract(SourceAST& ast) {
	ProcessNode(ast.getRoot());
}
