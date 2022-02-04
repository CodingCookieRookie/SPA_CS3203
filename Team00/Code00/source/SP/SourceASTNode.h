#pragma once

#include <string>
#include <vector>
#include <iostream>

class SourceASTNode {
public:
	SourceASTNode();

	// For debugging purpose.
	void printDashes(int depth) {
		for (int i = 0; i < depth; i++) {
			std::cout << "-";
		}
	}
	virtual void printNode(int depth) = 0;
};

class StmtNode : public SourceASTNode {
private:
	size_t stmtNum;
public:
	size_t getStmtNum();

	StmtNode(size_t stmtNum);

	void printNode(int depth = 1);

	friend class SourceAST;
};

class ReadNode : public StmtNode {
private:
	std::string varName;
public:
	ReadNode(std::string varName, size_t stmtNum);
	std::string getVarName();
	void printNode(int depth);

	friend class SourceAST;
};

class PrintNode : public StmtNode {
private:
	std::string varName;
public:
	PrintNode(std::string varName, size_t stmtNum);
	std::string getVarName();
	void printNode(int depth);

	friend class SourceAST;
};

class StmtListNode : public SourceASTNode {
private:
	std::vector<StmtNode*> stmtNodes;
public:
	StmtListNode();
	void addStmtNode(StmtNode* stmtNode);
	std::vector<StmtNode*> getStmtNodes();

	void printNode(int depth = 1);

	friend class SourceAST;
};

class ProcedureNode : public SourceASTNode {
private:
	std::string procName;
	StmtListNode* stmtListNode;
public:
	ProcedureNode(std::string procName);
	void addStmtList(StmtListNode* stmtListNode);
	StmtListNode* getStmtListNode();
	std::string getProcName();

	void printNode(int depth);

	friend class SourceAST;
};

class ProgramNode : public SourceASTNode {
private:
	std::vector<ProcedureNode*> procedureNodes;
public:
	static size_t currStmtNum;
	static void incrCurrStmtNum();

	ProgramNode();
	void addProcedure(ProcedureNode* procedureNode);
	std::vector<ProcedureNode*> getProcedureNodes();

	void printNode(int depth = 1);

	friend class SourceAST;
};
