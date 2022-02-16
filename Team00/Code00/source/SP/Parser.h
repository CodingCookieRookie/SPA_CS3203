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
	static WhileNode* matchWhile();
	static ExprNode* matchCondExpr();
	static ExprNode* matchRelExpr();
	static ExprNode* matchRelFactor();

	static const std::string WHILE;

	static const std::string NOT;
	static const std::string LEFT_BRACKET;
	static const std::string RIGHT_BRACKET;
	static const std::string LEFT_CURLY;
	static const std::string RIGHT_CURLY;

	static const std::vector<std::string> termOperators;
	static const std::vector<std::string> exprOperators;
	static const std::vector<std::string> relOperators;
	static const std::vector<std::string> logicalOperators;

public:
	Parser();
	static SourceAST parse(const std::string& source);
	static bool isValidAST(const SourceAST& ast);
};
