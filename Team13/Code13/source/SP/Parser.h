#pragma once

#include <string>
#include <vector>

#include "../Common/ExpressionParser.h"
#include "../Common/Lexer.h"
#include "../Exception/SPAException.h"
#include "SourceAST.h"
#include "SourceASTNode.h"

class Parser {
private:
	static Lexer lexer;

	static const std::string PROCEDURE;
	static const std::string READ;
	static const std::string PRINT;
	static const std::string WHILE;
	static const std::string IF;
	static const std::string THEN;
	static const std::string ELSE;
	static const std::string CALL;

	static const std::string EQUAL;
	static const std::string NOT;
	static const std::string LEFT_BRACKET;
	static const std::string RIGHT_BRACKET;
	static const std::string LEFT_CURLY;
	static const std::string RIGHT_CURLY;
	static const std::string SEMICOLON;

	static const std::vector<std::string> relOperators;
	static const std::vector<std::string> logicalOperators;

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
