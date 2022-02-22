#include "Parser.h"

Parser::Parser() {}

Lexer Parser::lexer;

const std::string Parser::PROCEDURE = "procedure";
const std::string Parser::READ = "read";
const std::string Parser::PRINT = "print";
const std::string Parser::WHILE = "while";
const std::string Parser::IF = "if";
const std::string Parser::THEN = "then";
const std::string Parser::ELSE = "else";

const std::string Parser::EQUAL = "=";
const std::string Parser::NOT = "!";
const std::string Parser::LEFT_BRACKET = "(";
const std::string Parser::RIGHT_BRACKET = ")";
const std::string Parser::LEFT_CURLY = "{";
const std::string Parser::RIGHT_CURLY = "}";
const std::string Parser::SEMICOLON = ";";

const std::vector<std::string> Parser::termOperators = { "*", "/", "%" };
const std::vector<std::string> Parser::exprOperators = { "+", "-" };
const std::vector<std::string> Parser::relOperators = { ">=", ">", "<=", "<", "!=", "==" };
const std::vector<std::string> Parser::logicalOperators = { "&&", "||" };

SourceAST Parser::parse(const std::string& source) {
	lexer = Lexer(source);
	ProgramNode* root = matchProgram();
	return SourceAST(root);
}

ProgramNode* Parser::matchProgram() {
	ProgramNode* programNode = new ProgramNode();

	ProcedureNode* procNode{};
	while (!lexer.reachedEnd()) {
		procNode = matchProcedure();
		programNode->addProcedure(procNode);
	}

	if (procNode == nullptr) {
		throw ParserException(ParserException::INVALID_PROG);
	}

	return programNode;
}

ProcedureNode* Parser::matchProcedure() {
	if (!lexer.match(PROCEDURE)) {
		throw ParserException(ParserException::MISSING_PROC_KEYWORD);
	}

	std::string whitespace = lexer.nextWhitespace();
	if (whitespace.empty()) {
		throw ParserException(ParserException::MISSING_WHITESPACE);
	}

	std::string procName = lexer.nextName();
	if (procName.empty()) {
		throw ParserException(ParserException::MISSING_PROC_NAME);
	}

	if (!lexer.match(LEFT_CURLY)) {
		throw ParserException(ParserException::MISSING_LEFT_CURLY);
	}

	StmtLstNode* stmtLstNode = matchStmtLst();

	if (!lexer.match(RIGHT_CURLY)) {
		throw ParserException(ParserException::MISSING_RIGHT_CURLY);
	}

	ProcedureNode* procNode = new ProcedureNode(procName);
	procNode->addStmtLst(stmtLstNode);

	return procNode;
}

StmtLstNode* Parser::matchStmtLst() {
	StmtLstNode* stmtLstNode = new StmtLstNode();

	StmtNode* stmtNode{};
	while (stmtNode = matchStmt()) {
		stmtLstNode->addStmtNode(stmtNode);
		if (lexer.peek(RIGHT_CURLY)) {
			break;
		}
	}
	return stmtLstNode;
}

StmtNode* Parser::matchStmt() {
	StmtNode* stmtNode{};

	std::string name = lexer.nextName();
	if (!name.empty() && lexer.match(EQUAL)) {
		stmtNode = matchAssign(name);
	} else if (name == READ) {
		stmtNode = matchRead();
	} else if (name == PRINT) {
		stmtNode = matchPrint();
	} else if (name == WHILE) {
		stmtNode = matchWhile();
	} else if (name == IF) {
		stmtNode = matchIf();
	} else {
		throw ParserException(ParserException::INVALID_STMT);
	}

	return stmtNode;
}

ReadNode* Parser::matchRead() {
	std::string varName = lexer.nextName();
	if (varName.empty()) {
		throw ParserException(ParserException::MISSING_VAR_NAME);
	}

	if (!lexer.match(SEMICOLON)) {
		throw ParserException(ParserException::MISSING_SEMICOLON);
	}

	return new ReadNode(varName);
}

PrintNode* Parser::matchPrint() {
	std::string varName = lexer.nextName();
	if (varName.empty()) {
		throw ParserException(ParserException::MISSING_VAR_NAME);
	}

	if (!lexer.match(SEMICOLON)) {
		throw ParserException(ParserException::MISSING_SEMICOLON);
	}

	return new PrintNode(varName);
}

ExprNode* Parser::matchFactor() {
	std::string varName = lexer.nextName();
	if (!varName.empty()) {
		return new ExprNode(ExprNodeValueType::varName, varName);
	}

	std::string constVal = lexer.nextInteger();
	if (!constVal.empty()) {
		return new ExprNode(ExprNodeValueType::constValue, constVal);
	}

	if (lexer.match(LEFT_BRACKET)) {
		ExprNode* expr = matchExpr();
		if (!lexer.match(RIGHT_BRACKET)) {
			throw ParserException(ParserException::INVALID_EXPR);
		}
		return expr;
	}

	throw ParserException(ParserException::INVALID_EXPR);
}

ExprNode* Parser::matchTermTail(ExprNode* lvalue) {
	for (const std::string op : termOperators) {
		if (lexer.match(op)) {
			ExprNode* factor = matchFactor();
			ExprNode* arithOp = new ExprNode(ExprNodeValueType::arithmeticOperator, op);
			arithOp->addChild(lvalue);
			arithOp->addChild(factor);
			return matchTermTail(arithOp);
		}
	}

	return lvalue;
}

ExprNode* Parser::matchTerm() {
	ExprNode* lvalue = matchFactor();
	return matchTermTail(lvalue);
}

ExprNode* Parser::matchExprTail(ExprNode* lvalue) {
	for (const std::string op : exprOperators) {
		if (lexer.match(op)) {
			ExprNode* term = matchTerm();
			ExprNode* arithOp = new ExprNode(ExprNodeValueType::arithmeticOperator, op);
			arithOp->addChild(lvalue);
			arithOp->addChild(term);
			return matchExprTail(arithOp);
		}
	}

	return lvalue;
}

ExprNode* Parser::matchExpr() {
	ExprNode* lvalue = matchTerm();
	return matchExprTail(lvalue);
}

AssignNode* Parser::matchAssign(std::string varName) {
	ExprNode* expr = matchExpr();

	if (!lexer.match(SEMICOLON)) {
		throw ParserException(ParserException::MISSING_SEMICOLON);
	}
	return new AssignNode(varName, expr);
}

WhileNode* Parser::matchWhile() {
	if (!lexer.match(LEFT_BRACKET)) {
		throw ParserException(ParserException::MISSING_LEFT_BRACKET);
	}

	ExprNode* condNode = matchCondExpr();

	if (!lexer.match(RIGHT_BRACKET)) {
		throw ParserException(ParserException::MISSING_RIGHT_BRACKET);
	}

	if (!lexer.match(LEFT_CURLY)) {
		throw ParserException(ParserException::MISSING_LEFT_CURLY);
	}

	StmtLstNode* stmtLstNode = matchStmtLst();

	if (!lexer.match(RIGHT_CURLY)) {
		throw ParserException(ParserException::MISSING_RIGHT_CURLY);
	}

	return new WhileNode(condNode, stmtLstNode);
}

ExprNode* Parser::matchCondExpr() {
	if (lexer.match(NOT)) {
		if (!lexer.match(LEFT_BRACKET)) {
			throw ParserException(ParserException::MISSING_LEFT_BRACKET);
		}

		ExprNode* condExpr = matchCondExpr();

		if (!lexer.match(RIGHT_BRACKET)) {
			throw ParserException(ParserException::MISSING_RIGHT_BRACKET);
		}

		ExprNode* notNode = new ExprNode(ExprNodeValueType::logicalOperator, NOT);
		notNode->addChild(condExpr);

		return notNode;
	}

	if (lexer.match(LEFT_BRACKET)) {
		ExprNode* leftCondExpr = matchCondExpr();

		if (!lexer.match(RIGHT_BRACKET)) {
			throw ParserException(ParserException::MISSING_RIGHT_BRACKET);
		}

		ExprNode* logOpNode{};
		for (const std::string op : logicalOperators) {
			if (lexer.match(op)) {
				logOpNode = new ExprNode(ExprNodeValueType::logicalOperator, op);
				break;
			}
		}

		if (logOpNode == nullptr) {
			throw ParserException(ParserException::INVALID_COND_EXPR);
		}

		if (!lexer.match(LEFT_BRACKET)) {
			throw ParserException(ParserException::MISSING_LEFT_BRACKET);
		}

		ExprNode* rightCondExpr = matchCondExpr();

		if (!lexer.match(RIGHT_BRACKET)) {
			throw ParserException(ParserException::MISSING_RIGHT_BRACKET);
		}

		logOpNode->addChild(leftCondExpr);
		logOpNode->addChild(rightCondExpr);

		return logOpNode;
	}

	ExprNode* relExprNode = matchRelExpr();

	return relExprNode;
}

ExprNode* Parser::matchRelExpr() {
	ExprNode* leftRelFactor = matchRelFactor();

	ExprNode* relOpNode{};
	for (const std::string op : relOperators) {
		if (lexer.match(op)) {
			relOpNode = new ExprNode(ExprNodeValueType::relOperator, op);
			break;
		}
	}
	if (relOpNode == nullptr) {
		throw ParserException(ParserException::INVALID_REL_EXPR);
	}

	ExprNode* rightRelFactor = matchRelFactor();

	relOpNode->addChild(leftRelFactor);
	relOpNode->addChild(rightRelFactor);
	return relOpNode;
}

ExprNode* Parser::matchRelFactor() {
	ExprNode* expr{};
	try {
		expr = matchExpr();
	} catch (ParserException& ex) {
		std::string varName = lexer.nextName();
		if (!varName.empty()) {
			return new ExprNode(ExprNodeValueType::varName, varName);
		}

		std::string constVal = lexer.nextInteger();
		if (!constVal.empty()) {
			return new ExprNode(ExprNodeValueType::constValue, constVal);
		}

		throw ParserException(ParserException::INVALID_REL_EXPR);
	}

	return expr;
}

IfNode* Parser::matchIf() {
	if (!lexer.match(LEFT_BRACKET)) {
		throw ParserException(ParserException::MISSING_LEFT_BRACKET);
	}

	ExprNode* condNode = matchCondExpr();

	if (!lexer.match(RIGHT_BRACKET)) {
		throw ParserException(ParserException::MISSING_RIGHT_BRACKET);
	}

	if (!lexer.match(THEN)) {
		throw ParserException(ParserException::MISSING_THEN);
	}

	if (!lexer.match(LEFT_CURLY)) {
		throw ParserException(ParserException::MISSING_LEFT_CURLY);
	}

	StmtLstNode* thenStmtLst = matchStmtLst();

	if (!lexer.match(RIGHT_CURLY)) {
		throw ParserException(ParserException::MISSING_RIGHT_CURLY);
	}

	if (!lexer.match(ELSE)) {
		throw ParserException(ParserException::MISSING_ELSE);
	}

	if (!lexer.match(LEFT_CURLY)) {
		throw ParserException(ParserException::MISSING_LEFT_CURLY);
	}

	StmtLstNode* elseStmtLst = matchStmtLst();

	if (!lexer.match(RIGHT_CURLY)) {
		throw ParserException(ParserException::MISSING_RIGHT_CURLY);
	}

	return new IfNode(condNode, thenStmtLst, elseStmtLst);
}
