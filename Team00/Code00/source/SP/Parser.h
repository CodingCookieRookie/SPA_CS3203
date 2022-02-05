#pragma once
#include <string>
#include <vector>

#include "../Commons/Lexer.h"
#include "SourceAST.h"
#include "SourceASTNode.h"

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
