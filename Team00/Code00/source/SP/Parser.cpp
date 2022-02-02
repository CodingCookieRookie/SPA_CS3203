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
	ProgramNode* programNode{};

	// There must exist at least 1 procedure
	ProcedureNode* procNode;
	while (procNode = matchProcedure()) {
		programNode->addChild(procNode);
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
	procNode->addChild(stmtListNode);

	return procNode;
}

/* stmtLst : stmt+ */
StmtListNode* Parser::matchStmtList() {
	StmtListNode* stmtListNode{};

	// There must exist at least 1 stmt
	StmtNode* stmtNode;
	while (stmtNode = matchStmt()) {
		stmtListNode->addChild(stmtNode);
	}

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

	return new ReadNode(varName);
}
