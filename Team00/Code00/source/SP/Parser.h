#pragma once
#include <string>
#include <vector>

#include "SourceAST.h"
#include "SourceASTNode.h"
#include "Lexer.h"

class Parser {
private:
	static std::vector<std::string> validStmts;
	Lexer lexer;
	ProgramNode* matchProgram();
	ProcedureNode* matchProcedure();
	StmtListNode* matchStmtList();
	StmtNode* matchStmt();
	ReadNode* matchRead();
	PrintNode* matchPrint();
public:
	Parser(const std::string& query);
	SourceAST parse();
	static bool isValidAST(const SourceAST& ast);
};
