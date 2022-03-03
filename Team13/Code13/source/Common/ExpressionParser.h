#pragma once

#include "Lexer.h"
#include "../SP/SourceASTNode.h"

class ExpressionParser {
private:
	ExpressionParser();

	static const std::string LEFT_BRACKET;
	static const std::string RIGHT_BRACKET;

	static const std::vector<std::string> termOperators;
	static const std::vector<std::string> exprOperators;
public:
	static ExprNode* matchFactor(Lexer& lexer);
	static ExprNode* matchTermTail(Lexer& lexer, ExprNode* lvalue);
	static ExprNode* matchTerm(Lexer& lexer);
	static ExprNode* matchExprTail(Lexer& lexer, ExprNode* lvalue);
	static ExprNode* matchExpr(Lexer& lexer);
};
