#include "ASTValidator.h"

void ASTValidator::validateAST(SourceAST& ast) {
	ProgramNode* progNode = ast.getRoot();
	std::unordered_set<std::string> procNames = getProcNames(progNode);

	std::unordered_map<std::string, std::unordered_set<std::string>> callsMap{};
	validateExistentProcCalls(progNode, callsMap, procNames);
	if (isCyclic(callsMap)) {
		throw ASTException(ASTException::CYCLIC_CALL_ERROR);
	}
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

		if (stmtNode->getStmtType() == StatementType::CALL_TYPE) {
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

bool ASTValidator::isCyclic(std::unordered_map<std::string, std::unordered_set<std::string>>& callsMap) {
	/* Initialise all nodes as not visited */
	std::unordered_map<std::string, CallNodeState> visited;
	for (const std::pair<std::string, std::unordered_set<std::string>>& callsPair : callsMap) {
		visited[callsPair.first] = CallNodeState::NOT_VISITED;
	}

	/* Iterate all the non-visited nodes */
	for (const std::pair<std::string, std::unordered_set<std::string>>& callsPair : callsMap) {
		std::string curr = callsPair.first;
		if (visited[curr] == CallNodeState::NOT_VISITED) {
			std::stack<std::string> stack;
			stack.push(curr);
			visited[curr] = CallNodeState::IN_STACK;
			if (isCyclicUtil(stack, visited, callsMap)) {
				return true;
			}
		}
	}
	return false;
}

bool ASTValidator::isCyclicUtil(
	std::stack<std::string>& stack,
	std::unordered_map<std::string, CallNodeState>& visited,
	std::unordered_map<std::string, std::unordered_set<std::string>>& callsMap) {
	std::unordered_set<std::string> calledProcs = callsMap[stack.top()];
	for (std::string calledProc : calledProcs) {
		if (visited[calledProc] == CallNodeState::IN_STACK) {
			return true;
		}

		if (visited[calledProc] == CallNodeState::NOT_VISITED) {
			stack.push(calledProc);
			visited[calledProc] = CallNodeState::IN_STACK;
			if (isCyclicUtil(stack, visited, callsMap)) {
				return true;
			}
		}
	}
	visited[stack.top()] = CallNodeState::DONE;
	stack.pop();
	return false;
}
