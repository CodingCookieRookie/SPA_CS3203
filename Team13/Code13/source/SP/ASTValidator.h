#pragma once

#include <stack>
#include <unordered_map>

#include "../Exception/SPAException.h"
#include "./SourceAST/ProcedureNode.h"
#include "./SourceAST/ProgramNode.h"
#include "./SourceAST/SourceAST.h"
#include "./SourceAST/StmtLstNode.h"
#include "./SourceAST/StmtNode.h"

enum class CallNodeState {
	NOT_VISITED, IN_STACK, DONE
};

class ASTValidator {
private:
	ASTValidator() {}

	/* Returns the list of proc names if there is no duplicate */
	static std::unordered_set<std::string> getProcNames(ProgramNode* progNode);

	/* Checks if all call stmts are calling existent procedures */
	static void validateExistentProcCalls(
		ProgramNode* progNode,
		std::unordered_map<std::string, std::unordered_set<std::string>>& callsMap,
		std::unordered_set<std::string>& procNames);
	static void processCallsStmtLstNode(
		StmtLstNode* stmtLstNode,
		std::unordered_set<std::string>& calledProcs,
		std::unordered_set<std::string>& procNames);
	static void processCallsStmtNode(
		StmtNode* stmtNode,
		std::unordered_set<std::string>& calledProcs,
		std::unordered_set<std::string>& procNames);

	/* Checks for cyclic calls */
	static bool isCyclic(std::unordered_map<std::string, std::unordered_set<std::string>>& callsMap);
	static bool isCyclicUtil(
		std::stack<std::string>& stack,
		std::unordered_map<std::string, CallNodeState>& visited,
		std::unordered_map<std::string, std::unordered_set<std::string>>& callsMap);

public:
	static void validateAST(SourceAST& ast);
};
