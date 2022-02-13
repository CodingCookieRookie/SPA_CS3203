#pragma once
#include <string>
#include <vector>

#include "../common/Lexer.h"
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
	static ExprNode* matchExpr();
	static AssignNode* matchAssign(std::string varName);
	static ExprNode* matchFactor();
	static ExprNode* matchTermTail(ExprNode* lvalue);
	static ExprNode* matchTerm();
	static ExprNode* matchExprTail(ExprNode* lvalue);

public:
	Parser();
	static SourceAST parse(const std::string& source);
	static bool isValidAST(const SourceAST& ast);
};
