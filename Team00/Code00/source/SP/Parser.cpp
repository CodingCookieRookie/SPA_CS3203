#include "Parser.h"

Parser::Parser() {}

Lexer Parser::lexer;

const std::string Parser::WHILE = "while";
const std::string Parser::IF = "if";
const std::string Parser::THEN = "then";
const std::string Parser::ELSE = "else";

const std::string Parser::NOT = "!";
const std::string Parser::LEFT_BRACKET = "(";
const std::string Parser::RIGHT_BRACKET = ")";
const std::string Parser::LEFT_CURLY = "{";
const std::string Parser::RIGHT_CURLY = "}";

const std::vector<std::string> Parser::termOperators = { "*", "/", "%" };
const std::vector<std::string> Parser::exprOperators = { "+", "-" };
const std::vector<std::string> Parser::relOperators = { ">=", ">", "<=", "<", "!=", "==" };
const std::vector<std::string> Parser::logicalOperators = { "&&", "||" };

SourceAST Parser::parse(const std::string& source) {
	lexer = Lexer(source);
	ProgramNode* root = matchProgram();
	return SourceAST(root);
}

bool Parser::isValidAST(const SourceAST& pqlAst) {
	return true; // TODO: to change
}

/* program : procedure+ */
ProgramNode* Parser::matchProgram() {
	ProgramNode* programNode = new ProgramNode();

	// There must exist at least 1 procedure
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

/* procedure : ‘procedure’ proc_name ‘{‘ stmtLst ‘}’ */
ProcedureNode* Parser::matchProcedure() {
	if (!lexer.match("procedure")) {
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

	if (!lexer.match("{")) {
		throw ParserException(ParserException::MISSING_LEFT_CURLY);
	}

	StmtLstNode* stmtLstNode = matchStmtLst();

	if (!lexer.match("}")) {
		throw ParserException(ParserException::MISSING_RIGHT_CURLY);
	}

	ProcedureNode* procNode = new ProcedureNode(procName);
	procNode->addStmtLst(stmtLstNode);

	return procNode;
}

/* stmtLst : stmt+ */
StmtLstNode* Parser::matchStmtLst() {
	StmtLstNode* stmtLstNode = new StmtLstNode();

	// There must exist at least 1 stmt.
	StmtNode* stmtNode{};
	while (stmtNode = matchStmt()) {
		stmtLstNode->addStmtNode(stmtNode);
		if (lexer.peek("}")) {
			break;
		}
	}
	return stmtLstNode;
}

/* stmt : read | print | call | while | if | assign */
StmtNode* Parser::matchStmt() {
	bool isValidStmt = false;
	StmtNode* stmtNode{};

	// TODO: make this more elegant. Add invalid statement handler.
	if (lexer.match("read")) {
		stmtNode = matchRead();
	} else if (lexer.match("print")) {
		stmtNode = matchPrint();
	} else if (lexer.match(WHILE)) {
		stmtNode = matchWhile();
	} else if (lexer.match(IF)) {
		stmtNode = matchIf();
	} else {
		std::string varName = lexer.nextName();
		if (!varName.empty() && lexer.match("=")) {
			stmtNode = matchAssign(varName);
		} else {
			throw ParserException(ParserException::INVALID_STMT);
		}
	}

	return stmtNode;
}

/* read : ‘read’ var_name; */
ReadNode* Parser::matchRead() {
	std::string whitespace = lexer.nextWhitespace();
	if (whitespace.empty()) {
		throw ParserException(ParserException::MISSING_WHITESPACE);
	}

	std::string varName = lexer.nextName();
	if (varName.empty()) {
		throw ParserException(ParserException::MISSING_VAR_NAME);
	}

	if (!lexer.match(";")) {
		throw ParserException(ParserException::MISSING_SEMICOLON);
	}

	return new ReadNode(varName);
}

/* print : ‘print’ var_name; */
PrintNode* Parser::matchPrint() {
	std::string whitespace = lexer.nextWhitespace();
	if (whitespace.empty()) {
		throw ParserException(ParserException::MISSING_WHITESPACE);
	}

	std::string varName = lexer.nextName();
	if (varName.empty()) {
		throw ParserException(ParserException::MISSING_VAR_NAME);
	}

	if (!lexer.match(";")) {
		throw ParserException(ParserException::MISSING_SEMICOLON);
	}

	return new PrintNode(varName);
}

ExprNode* matchExpr();

/* assign: var_name '=' expr ';' */
AssignNode* Parser::matchAssign(std::string varName) {
	ExprNode* expr = matchExpr();

	if (!lexer.match(";")) {
		throw ParserException(ParserException::MISSING_SEMICOLON);
	}
	return new AssignNode(varName, expr);
}

/* <factor> ::= var_name | const_value | '(' expr ')' */
ExprNode* Parser::matchFactor() {
	std::string varName = lexer.nextName();
	if (!varName.empty()) {
		return new ExprNode(ExprNodeValueType::varName, varName);
	}

	std::string constVal = lexer.nextInteger();
	if (!constVal.empty()) {
		return new ExprNode(ExprNodeValueType::constValue, constVal);
	}

	if (lexer.match("(")) {
		ExprNode* expr = matchExpr();
		if (!lexer.match(")")) {
			throw ParserException(ParserException::INVALID_EXPR);
		}
		return expr;
	}

	throw ParserException(ParserException::INVALID_EXPR);
}

/* <term_tail> ::= * <factor> <term_tail>
			  | / <factor> <term_tail>
			  | % <factor> <term_tail>
			  | <empty>
*/
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

/* <term> ::= <factor> <term_tail> */
ExprNode* Parser::matchTerm() {
	ExprNode* lvalue = matchFactor();
	return matchTermTail(lvalue);
}

/*
<expr_tail> ::= + <term> <expr_tail>
			  | - <term> <expr_tail>
			  | <empty>

 */
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

/* <expr> ::= <term> <expr_tail> */
ExprNode* Parser::matchExpr() {
	ExprNode* lvalue = matchTerm();
	return matchExprTail(lvalue);
}

/* while : ‘while’ ‘(’ cond_expr ‘)’ ‘{‘ stmtLst ‘}’ */
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

/* cond_expr : rel_expr
			| ‘!’ ‘(’ cond_expr ‘)’
			| ‘(’ cond_expr ‘)’ ‘&&’ ‘(’ cond_expr ‘)’
			| ‘(’ cond_expr ‘)’ ‘||’ ‘(’ cond_expr ‘)’
*/
ExprNode* Parser::matchCondExpr() {
	/* ‘!’ ‘(’ cond_expr ‘)’*/
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

	/* ‘(’ cond_expr ‘)’ ‘&&’ ‘(’ cond_expr ‘)’
		| ‘(’ cond_expr ‘)’ ‘||’ ‘(’ cond_expr ‘)’ */
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

	/* rel_expr */
	ExprNode* relExprNode = matchRelExpr();

	return relExprNode;
}

/* rel_expr: rel_factor ‘ > ’ rel_factor
			| rel_factor ‘ >= ’ rel_factor
			| rel_factor ‘ < ’ rel_factor
			| rel_factor ‘ <= ’ rel_factor
			| rel_factor ‘ == ’ rel_factor
			| rel_factor ‘ != ’ rel_factor
*/
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

/* rel_factor : var_name
				| const_value
				| expr
*/
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

/* if : ‘if’ ‘(’ cond_expr ‘)’ ‘then’ ‘{‘ stmtLst ‘}’ ‘else’ ‘{‘ stmtLst ‘}’ */
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
