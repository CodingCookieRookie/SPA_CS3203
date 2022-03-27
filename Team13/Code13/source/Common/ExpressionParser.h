#pragma once

#include "Common.h"
#include "Lexer.h"
#include "../SP/SourceASTNode.h"

class ExpressionParser {
private:
	ExpressionParser();

	static const std::vector<std::string> TERM_OPERATORS;
	static const std::vector<std::string> EXPR_OPERATORS;
public:
	static ExprNode* matchFactor(Lexer& lexer);
	static ExprNode* matchTermTail(Lexer& lexer, ExprNode* lvalue);
	static ExprNode* matchTerm(Lexer& lexer);
	static ExprNode* matchExprTail(Lexer& lexer, ExprNode* lvalue);
	static ExprNode* matchExpr(Lexer& lexer);
};
