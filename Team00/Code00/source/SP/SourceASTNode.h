#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>

#include "../common/Types.h"

/* Forward definition of StmtLstNode for compatibility with StmtNode */
class StmtLstNode;

class SourceASTNode {
public:
	SourceASTNode();
};

class StmtNode : public SourceASTNode {
public:
	StmtNode();
	virtual StatementType getStmtType() = 0;
	virtual std::unordered_set<std::string> getModifiesVars();
	virtual std::unordered_set<std::string> getUsesVars();
	virtual std::unordered_set<std::string> getUsesConsts();
	virtual std::string getPattern();
	virtual std::vector<StmtLstNode*> getChildStmtLst();

	friend class SourceAST;
};

class ReadNode : public StmtNode {
private:
	std::string varName;
public:
	ReadNode(std::string varName);
	std::string getVarName();
	StatementType getStmtType();
	std::unordered_set<std::string> getModifiesVars();

	friend class SourceAST;
};

class PrintNode : public StmtNode {
private:
	std::string varName;
public:
	PrintNode(std::string varName);
	std::string getVarName();
	StatementType getStmtType();
	std::unordered_set<std::string> getUsesVars();

	friend class SourceAST;
};

class StmtLstNode : public SourceASTNode {
private:
	std::vector<StmtNode*> stmtNodes;
public:
	StmtLstNode();
	void addStmtNode(StmtNode* stmtNode);
	std::vector<StmtNode*> getStmtNodes();

	friend class SourceAST;
};

class ProcedureNode : public SourceASTNode {
private:
	std::string procName;
	StmtLstNode* stmtLstNode;
public:
	ProcedureNode(std::string procName);
	void addStmtLst(StmtLstNode* stmtLstNode);
	StmtLstNode* getStmtLstNode();
	std::string getProcName();

	friend class SourceAST;
};

class ProgramNode : public SourceASTNode {
private:
	std::vector<ProcedureNode*> procedureNodes;
public:
	ProgramNode();
	void addProcedure(ProcedureNode* procedureNode);
	std::vector<ProcedureNode*> getProcedureNodes();

	friend class SourceAST;
};
