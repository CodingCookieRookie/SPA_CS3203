#pragma once

#include <string>
#include <vector>

#include "../Common/Common.h"
#include "../Common/ExpressionParser.h"
#include "../Common/Lexer.h"
#include "../Exception/SPAException.h"
#include "./SourceAST/ProcedureNode.h"
#include "./SourceAST/ProgramNode.h"
#include "./SourceAST/SourceAST.h"
#include "./SourceAST/StmtLstNode.h"
#include "./SourceAST/StmtNode.h"

class Parser {
private:
	static Lexer lexer;

	ProgramNode* matchProgram();
	ProcedureNode* matchProcedure();
	StmtLstNode* matchStmtLst();
	StmtNode* matchStmt();
	ReadNode* matchRead();
	PrintNode* matchPrint();
	AssignNode* matchAssign(std::string varName);
	WhileNode* matchWhile();
	ExprNode* matchCondExpr();
	ExprNode* matchRelExpr();
	ExprNode* matchRelFactor();
	IfNode* matchIf();
	CallNode* matchCall();

	StmtIndex stmtIdx;

public:
	Parser();
	SourceAST parse(const std::string& source);
};
