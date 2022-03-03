#include "ExpressionParser.h"

const std::string ExpressionParser::LEFT_BRACKET = "(";
const std::string ExpressionParser::RIGHT_BRACKET = ")";

const std::vector<std::string> ExpressionParser::termOperators = { "*", "/", "%" };
const std::vector<std::string> ExpressionParser::exprOperators = { "+", "-" };

ExprNode* ExpressionParser::matchFactor(Lexer& lexer) {
	std::string varName = lexer.nextName();
	if (!varName.empty()) {
		return new ExprNode(ExprNodeValueType::varName, varName);
	}

	std::string constVal = lexer.nextInteger();
	if (!constVal.empty()) {
		return new ExprNode(ExprNodeValueType::constValue, constVal);
	}

	if (lexer.match(LEFT_BRACKET)) {
		ExprNode* expr = matchExpr(lexer);
		if (!lexer.match(RIGHT_BRACKET)) {
			throw ExpressionException(ExpressionException::INVALID_EXPR);
		}
		return expr;
	}

	throw ExpressionException(ExpressionException::INVALID_EXPR);
}

ExprNode* ExpressionParser::matchTermTail(Lexer& lexer, ExprNode* lvalue) {
	for (const std::string op : termOperators) {
		if (lexer.match(op)) {
			ExprNode* factor = matchFactor(lexer);
			ExprNode* arithOp = new ExprNode(ExprNodeValueType::arithmeticOperator, op);
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
	for (const std::string op : exprOperators) {
		if (lexer.match(op)) {
			ExprNode* term = matchTerm(lexer);
			ExprNode* arithOp = new ExprNode(ExprNodeValueType::arithmeticOperator, op);
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
