#include "Parser.h"

Parser::Parser() {}

Lexer Parser::lexer;

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
	if (!lexer.match(Common::PROCEDURE)) {
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

	if (!lexer.match(Common::LEFT_CURLY)) {
		throw ParserException(ParserException::MISSING_LEFT_CURLY);
	}

	StmtLstNode* stmtLstNode = matchStmtLst();

	if (!lexer.match(Common::RIGHT_CURLY)) {
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
		if (lexer.peek(Common::RIGHT_CURLY)) {
			break;
		}
	}
	return stmtLstNode;
}

StmtNode* Parser::matchStmt() {
	StmtNode* stmtNode{};

	std::string name = lexer.nextName();
	if (!name.empty() && lexer.match(Common::EQUAL)) {
		stmtNode = matchAssign(name);
	} else if (name == Common::READ) {
		stmtNode = matchRead();
	} else if (name == Common::PRINT) {
		stmtNode = matchPrint();
	} else if (name == Common::WHILE) {
		stmtNode = matchWhile();
	} else if (name == Common::IF) {
		stmtNode = matchIf();
	} else if (name == Common::CALL) {
		stmtNode = matchCall();
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

	if (!lexer.match(Common::SEMICOLON)) {
		throw ParserException(ParserException::MISSING_SEMICOLON);
	}

	return new ReadNode(varName);
}

PrintNode* Parser::matchPrint() {
	std::string varName = lexer.nextName();
	if (varName.empty()) {
		throw ParserException(ParserException::MISSING_VAR_NAME);
	}

	if (!lexer.match(Common::SEMICOLON)) {
		throw ParserException(ParserException::MISSING_SEMICOLON);
	}

	return new PrintNode(varName);
}

AssignNode* Parser::matchAssign(std::string varName) {
	ExprNode* expr = nullptr;
	try {
		expr = ExpressionParser::matchExpr(lexer);
	} catch (ExpressionException& ex) {
		throw ParserException(ParserException::INVALID_EXPR);
	}

	if (!lexer.match(Common::SEMICOLON)) {
		throw ParserException(ParserException::MISSING_SEMICOLON);
	}
	return new AssignNode(varName, expr);
}

WhileNode* Parser::matchWhile() {
	if (!lexer.match(Common::LEFT_BRACKET)) {
		throw ParserException(ParserException::MISSING_LEFT_BRACKET);
	}

	ExprNode* condNode = matchCondExpr();

	if (!lexer.match(Common::RIGHT_BRACKET)) {
		throw ParserException(ParserException::MISSING_RIGHT_BRACKET);
	}

	if (!lexer.match(Common::LEFT_CURLY)) {
		throw ParserException(ParserException::MISSING_LEFT_CURLY);
	}

	StmtLstNode* stmtLstNode = matchStmtLst();

	if (!lexer.match(Common::RIGHT_CURLY)) {
		throw ParserException(ParserException::MISSING_RIGHT_CURLY);
	}

	return new WhileNode(condNode, stmtLstNode);
}

ExprNode* Parser::matchCondExpr() {
	if (lexer.match(Common::NOT)) {
		if (!lexer.match(Common::LEFT_BRACKET)) {
			throw ParserException(ParserException::MISSING_LEFT_BRACKET);
		}

		ExprNode* condExpr = matchCondExpr();

		if (!lexer.match(Common::RIGHT_BRACKET)) {
			throw ParserException(ParserException::MISSING_RIGHT_BRACKET);
		}

		ExprNode* notNode = new ExprNode(ExprNodeValueType::LOGICAL_OPERATOR, Common::NOT);
		notNode->addChild(condExpr);

		return notNode;
	}

	if (lexer.match(Common::LEFT_BRACKET)) {
		ExprNode* leftCondExpr = matchCondExpr();

		if (!lexer.match(Common::RIGHT_BRACKET)) {
			throw ParserException(ParserException::MISSING_RIGHT_BRACKET);
		}

		ExprNode* logOpNode{};
		for (const std::string op : Common::LOGICAL_OPERATORS) {
			if (lexer.match(op)) {
				logOpNode = new ExprNode(ExprNodeValueType::LOGICAL_OPERATOR, op);
				break;
			}
		}

		if (logOpNode == nullptr) {
			throw ParserException(ParserException::INVALID_COND_EXPR);
		}

		if (!lexer.match(Common::LEFT_BRACKET)) {
			throw ParserException(ParserException::MISSING_LEFT_BRACKET);
		}

		ExprNode* rightCondExpr = matchCondExpr();

		if (!lexer.match(Common::RIGHT_BRACKET)) {
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
	for (const std::string op : Common::REL_OPERATORS) {
		if (lexer.match(op)) {
			relOpNode = new ExprNode(ExprNodeValueType::REL_OPERATOR, op);
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
		expr = ExpressionParser::matchExpr(lexer);
	} catch (ExpressionException& ex) {
		std::string varName = lexer.nextName();
		if (!varName.empty()) {
			return new ExprNode(ExprNodeValueType::VAR_NAME, varName);
		}

		std::string constVal = lexer.nextInteger();
		if (!constVal.empty()) {
			return new ExprNode(ExprNodeValueType::CONST_VALUE, constVal);
		}

		throw ParserException(ParserException::INVALID_REL_EXPR);
	}

	return expr;
}

IfNode* Parser::matchIf() {
	if (!lexer.match(Common::LEFT_BRACKET)) {
		throw ParserException(ParserException::MISSING_LEFT_BRACKET);
	}

	ExprNode* condNode = matchCondExpr();

	if (!lexer.match(Common::RIGHT_BRACKET)) {
		throw ParserException(ParserException::MISSING_RIGHT_BRACKET);
	}

	if (!lexer.match(Common::THEN)) {
		throw ParserException(ParserException::MISSING_THEN);
	}

	if (!lexer.match(Common::LEFT_CURLY)) {
		throw ParserException(ParserException::MISSING_LEFT_CURLY);
	}

	StmtLstNode* thenStmtLst = matchStmtLst();

	if (!lexer.match(Common::RIGHT_CURLY)) {
		throw ParserException(ParserException::MISSING_RIGHT_CURLY);
	}

	if (!lexer.match(Common::ELSE)) {
		throw ParserException(ParserException::MISSING_ELSE);
	}

	if (!lexer.match(Common::LEFT_CURLY)) {
		throw ParserException(ParserException::MISSING_LEFT_CURLY);
	}

	StmtLstNode* elseStmtLst = matchStmtLst();

	if (!lexer.match(Common::RIGHT_CURLY)) {
		throw ParserException(ParserException::MISSING_RIGHT_CURLY);
	}

	return new IfNode(condNode, thenStmtLst, elseStmtLst);
}

CallNode* Parser::matchCall() {
	std::string procName = lexer.nextName();
	if (procName.empty()) {
		throw ParserException(ParserException::MISSING_PROC_NAME_IN_CALL_STMT);
	}

	if (!lexer.match(Common::SEMICOLON)) {
		throw ParserException(ParserException::MISSING_SEMICOLON);
	}

	return new CallNode(procName);
}
