#pragma once

#include <string>
#include <vector>

#include "../Common/Common.h"
#include "../Common/ExpressionParser.h"
#include "../Common/Lexer.h"
#include "../Exception/SPAException.h"
#include "SourceAST.h"
#include "SourceASTNode.h"

class Parser {
private:
	static Lexer lexer;

	static ProgramNode* matchProgram();
	static ProcedureNode* matchProcedure();
	static StmtLstNode* matchStmtLst();
	static StmtNode* matchStmt();
	static ReadNode* matchRead();
	static PrintNode* matchPrint();
	static AssignNode* matchAssign(std::string varName);
	static WhileNode* matchWhile();
	static ExprNode* matchCondExpr();
	static ExprNode* matchRelExpr();
	static ExprNode* matchRelFactor();
	static IfNode* matchIf();
	static CallNode* matchCall();

public:
	Parser();
	static SourceAST parse(const std::string& source);
};
