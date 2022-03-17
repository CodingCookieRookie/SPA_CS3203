#pragma once

#include <unordered_map>

#include "../Exception/SPAException.h"
#include "SourceAST.h"

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
public:
	static void validateAST(SourceAST& ast);
};
