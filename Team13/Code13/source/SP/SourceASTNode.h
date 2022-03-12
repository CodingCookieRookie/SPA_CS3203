#pragma once

#include <queue>
#include <stack>
#include <string>
#include <unordered_set>
#include <vector>

#include "../Common/ExprNode.h"
#include "../Common/Types.h"

/* Forward definition of StmtLstNode for compatibility with StmtNode */
class StmtLstNode;
class ExprNode;

class SourceASTNode {
public:
	SourceASTNode();
};

class StmtNode : public SourceASTNode {
protected:
	std::unordered_set<std::string> getUsesVarsInExpr(ExprNode* expr);
	std::unordered_set<std::string> getConstsInExpr(ExprNode* expr);
public:
	StmtNode();
	virtual StatementType getStmtType() = 0;
	virtual std::string getPattern();
	virtual std::vector<StmtLstNode*> getChildStmtLst();

	/* Returns an unordered set of vars used by the stmt node */
	virtual std::unordered_set<std::string> getUsesVars();

	/* Returns an unordered set of vars modified by the stmt node */
	virtual std::unordered_set<std::string> getModifiesVars();

	/* Returns an unordered set of consts in the stmt node */
	virtual std::unordered_set<std::string> getConsts();

	/* Returns the procedure name called by the stmt node */
	virtual std::string getProcCalled();

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

class AssignNode : public StmtNode {
private:
	std::string varName;
	ExprNode* expr;
	std::unordered_set<std::string> usesVars;
	std::unordered_set<std::string> usesConsts;
	std::string pattern;
public:
	AssignNode(std::string varName, ExprNode* expr);
	std::string getVarName();
	ExprNode* getExpr();
	StatementType getStmtType();
	std::unordered_set<std::string> getUsesVars();
	std::unordered_set<std::string> getModifiesVars();
	std::unordered_set<std::string> getConsts();
	std::string getPattern();

	friend class SourceAST;
};

class ContainerNode : public StmtNode {
protected:
	ExprNode* condExpr;
	std::vector<StmtLstNode*> childStmtLst;
public:
	ContainerNode(ExprNode* condExpr, std::vector<StmtLstNode*> childStmtLst);
	ExprNode* getCondExpr();
	std::unordered_set<std::string> getUsesVars();
	std::unordered_set<std::string> getConsts();
	std::vector<StmtLstNode*> getChildStmtLst();
};

class WhileNode : public ContainerNode {
public:
	WhileNode(ExprNode* condExpr, StmtLstNode* stmtLst);
	StatementType getStmtType();
};

class IfNode : public ContainerNode {
public:
	IfNode(ExprNode* condExpr, StmtLstNode* thenStmtLst, StmtLstNode* elseStmtLst);
	StatementType getStmtType();
};

class CallNode : public StmtNode {
private:
	std::string procName;
public:
	CallNode(std::string procName);
	StatementType getStmtType();
	std::string getProcCalled();

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
