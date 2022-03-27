#include "ExpressionParser.h"

const std::vector<std::string> ExpressionParser::TERM_OPERATORS = { "*", "/", "%" };
const std::vector<std::string> ExpressionParser::EXPR_OPERATORS = { "+", "-" };

ExprNode* ExpressionParser::matchFactor(Lexer& lexer) {
	std::string varName = lexer.nextName();
	if (!varName.empty()) {
		return new ExprNode(ExprNodeValueType::VAR_NAME, varName);
	}

	std::string constVal = lexer.nextInteger();
	if (!constVal.empty()) {
		return new ExprNode(ExprNodeValueType::CONST_VALUE, constVal);
	}

	if (lexer.match(Common::LEFT_BRACKET)) {
		ExprNode* expr = matchExpr(lexer);
		if (!lexer.match(Common::RIGHT_BRACKET)) {
			throw ExpressionException(ExpressionException::INVALID_EXPR);
		}
		return expr;
	}

	throw ExpressionException(ExpressionException::INVALID_EXPR);
}

ExprNode* ExpressionParser::matchTermTail(Lexer& lexer, ExprNode* lvalue) {
	for (const std::string op : TERM_OPERATORS) {
		if (lexer.match(op)) {
			ExprNode* factor = matchFactor(lexer);
			ExprNode* arithOp = new ExprNode(ExprNodeValueType::ARITHMETIC_OPERATOR, op);
			arithOp->addChild(lvalue);
			arithOp->addChild(factor);
			return matchTermTail(lexer, arithOp);
		}
	}

	return lvalue;
}

ExprNode* ExpressionParser::matchTerm(Lexer& lexer) {
	ExprNode* lvalue = matchFactor(lexer);
	return matchTermTail(lexer, lvalue);
}

ExprNode* ExpressionParser::matchExprTail(Lexer& lexer, ExprNode* lvalue) {
	for (const std::string op : EXPR_OPERATORS) {
		if (lexer.match(op)) {
			ExprNode* term = matchTerm(lexer);
			ExprNode* arithOp = new ExprNode(ExprNodeValueType::ARITHMETIC_OPERATOR, op);
			arithOp->addChild(lvalue);
			arithOp->addChild(term);
			return matchExprTail(lexer, arithOp);
		}
	}

	return lvalue;
}

ExprNode* ExpressionParser::matchExpr(Lexer& lexer) {
	ExprNode* lvalue = matchTerm(lexer);
	return matchExprTail(lexer, lvalue);
}
