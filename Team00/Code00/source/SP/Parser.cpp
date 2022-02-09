#include "Parser.h"

Parser::Parser() {}

Lexer Parser::lexer;

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
		if (procNode == nullptr) {
			return nullptr;
		}
	}

	if (procNode == nullptr) {
		return nullptr;
	}

	return programNode;
}

/* procedure : ‘procedure’ proc_name ‘{‘ stmtLst ‘}’ */
ProcedureNode* Parser::matchProcedure() {
	if (!lexer.match("procedure")) {
		return nullptr;
	}

	std::string whitespace = lexer.nextWhitespace();
	if (whitespace.empty()) {
		return nullptr;
	}

	std::string procName = lexer.nextName();
	if (procName.empty()) {
		return nullptr;
	}

	if (!lexer.match("{")) {
		return nullptr;
	}

	StmtLstNode* stmtLstNode = matchStmtLst();
	if (stmtLstNode == nullptr) {
		return nullptr;
	}

	if (!lexer.match("}")) {
		return nullptr;
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
		throw ParserException(ParserException::INVALID_STATEMENT);
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
