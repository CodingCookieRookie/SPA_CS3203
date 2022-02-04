#include "Parser.h"

std::vector<std::string> Parser::validStmts = {
	"read", "print", "call", "while", "if", "assign"
};

Parser::Parser(const std::string& source) : lexer(source) { }

SourceAST Parser::parse() {
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
	ProcedureNode* procNode;
	while (!lexer.reachedEnd()) {
		procNode = matchProcedure();
		programNode->addProcedure(procNode);
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

	whitespace = lexer.nextWhitespace();
	if (whitespace.empty()) {
		return nullptr;
	}

	if (!lexer.match("{")) {
		return nullptr;
	}

	StmtListNode* stmtListNode = matchStmtList();
	if (stmtListNode == nullptr) {
		return nullptr;
	}

	if (!lexer.match("}")) {
		return nullptr;
	}

	ProcedureNode* procNode = new ProcedureNode(procName);
	procNode->addStmtList(stmtListNode);

	return procNode;
}

/* stmtLst : stmt+ */
StmtListNode* Parser::matchStmtList() {
	StmtListNode* stmtListNode = new StmtListNode();

	// There must exist at least 1 stmt
	StmtNode* stmtNode;
	while (!lexer.peek("}") && !lexer.reachedEnd()) {
		stmtNode = matchStmt();
		stmtListNode->addStmtNode(stmtNode);
		ProgramNode::incrCurrStmtNum();
	}

	// If any of the stmtNode is invalid, immediately terminate.
	if (stmtNode == nullptr) {
		return nullptr;
	}

	return stmtListNode;
}

/* stmt : read | print | call | while | if | assign */
StmtNode* Parser::matchStmt() {
	bool isValidStmt = false;
	std::string validStmt;
	StmtNode* stmtNode{};

	for (std::string& validStmt : validStmts) {
		// TODO: make this more elegant.
		if (lexer.match(validStmt)) {
			if (validStmt == "read") {
				stmtNode = matchRead();
			} else if (validStmt == "print") {
				stmtNode = matchPrint();
			}

			isValidStmt = true;
			break;
		}
	}

	if (!isValidStmt) {
		return nullptr;
	}

	return stmtNode;
}

/* read : ‘read’ var_name; */
ReadNode* Parser::matchRead() {
	std::string whitespace = lexer.nextWhitespace();
	if (whitespace.empty()) {
		return nullptr;
	}

	std::string varName = lexer.nextName();
	if (varName.empty()) {
		return nullptr;
	}

	if (!lexer.match(";")) {
		return nullptr;
	}

	return new ReadNode(varName, ProgramNode::currStmtNum);
}

/* print : ‘print’ var_name; */
PrintNode* Parser::matchPrint() {
	std::string whitespace = lexer.nextWhitespace();
	if (whitespace.empty()) {
		return nullptr;
	}

	std::string varName = lexer.nextName();
	if (varName.empty()) {
		return nullptr;
	}

	if (!lexer.match(";")) {
		return nullptr;
	}

	return new PrintNode(varName, ProgramNode::currStmtNum);
}
