#include "Parser.h"

Parser::Parser() {}

Lexer Parser::lexer;

std::vector<std::string> termOperands = { "*", "/", "%" };
std::vector<std::string> exprOperands = { "+", "-" };

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
		return new ExprNode(varName);
	}

	std::string constVal = lexer.nextInteger();
	if (!constVal.empty()) {
		return new ExprNode(constVal);
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
	for (const std::string op : termOperands) {
		if (lexer.match(op)) {
			ExprNode* factor = matchFactor();
			ExprNode* operand = new ExprNode(op);
			operand->addChild(lvalue);
			operand->addChild(factor);
			return matchTermTail(operand);
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
	for (const std::string op : exprOperands) {
		if (lexer.match(op)) {
			ExprNode* term = matchTerm();
			ExprNode* operand = new ExprNode(op);
			operand->addChild(lvalue);
			operand->addChild(term);
			return matchExprTail(operand);
		}
	}

	return lvalue;
}

/* <expr> ::= <term> <expr_tail> */
ExprNode* Parser::matchExpr() {
	ExprNode* lvalue = matchTerm();
	return matchExprTail(lvalue);
}
