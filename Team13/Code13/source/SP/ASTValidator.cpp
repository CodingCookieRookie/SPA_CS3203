#include "ASTValidator.h"

void ASTValidator::validateAST(SourceAST& ast) {
	ProgramNode* progNode = ast.getRoot();
	std::unordered_set<std::string> procNames = getProcNames(progNode);

	std::unordered_map<std::string, std::unordered_set<std::string>> callsMap{};
	validateExistentProcCalls(progNode, callsMap, procNames);
}

std::unordered_set<std::string> ASTValidator::getProcNames(ProgramNode* progNode) {
	std::unordered_set<std::string> procNames{};
	for (ProcedureNode* procedureNode : progNode->getProcedureNodes()) {
		/* If duplicate proc name is detected, terminate the program immediately */
		if (procNames.find(procedureNode->getProcName()) != procNames.end()) {
			throw ASTException(ASTException::DUPLICATE_PROC_NAMES_ERROR);
		}
		procNames.insert(procedureNode->getProcName());
	}
	return procNames;
}

void ASTValidator::validateExistentProcCalls(
	ProgramNode* progNode,
	std::unordered_map<std::string, std::unordered_set<std::string>>& callsMap,
	std::unordered_set<std::string>& procNames) {
	for (ProcedureNode* procedureNode : progNode->getProcedureNodes()) {
		std::unordered_set<std::string> calledProcs;
		processCallsStmtLstNode(procedureNode->getStmtLstNode(), calledProcs, procNames);
		callsMap[procedureNode->getProcName()] = calledProcs;
	}
}

void ASTValidator::processCallsStmtLstNode(
	StmtLstNode* stmtLstNode,
	std::unordered_set<std::string>& calledProcs,
	std::unordered_set<std::string>& procNames) {
	for (StmtNode* stmtNode : stmtLstNode->getStmtNodes()) {
		std::vector<StmtLstNode*> childStmtLsts = stmtNode->getChildStmtLst();
		for (StmtLstNode* stmtLstNode : childStmtLsts) {
			processCallsStmtLstNode(stmtLstNode, calledProcs, procNames);
		}

		if (stmtNode->getStmtType() == StatementType::callType) {
			processCallsStmtNode(stmtNode, calledProcs, procNames);
		}
	}
}

void ASTValidator::processCallsStmtNode(
	StmtNode* stmtNode,
	std::unordered_set<std::string>& calledProcs,
	std::unordered_set<std::string>& procNames) {
	std::string procName = stmtNode->getProcCalled();

	/* If call stmt is calling a non-existent proc, terminate the program immediately */
	if (procNames.find(procName) == procNames.end()) {
		throw ASTException(ASTException::CALL_NON_EXISTENT_PROC_NAME_ERROR);
	}

	calledProcs.insert(procName);
}
