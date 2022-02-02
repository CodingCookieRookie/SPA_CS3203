#pragma once

#include <string>
#include <vector>
#include <iostream>

class SourceASTNode {
protected:
	std::string value;

public:
	std::vector<SourceASTNode*> children{};

	SourceASTNode();
	SourceASTNode(std::string& value);
	void addChild(SourceASTNode* node);
	std::vector<SourceASTNode*> getChildren();
	std::string getValue();

	// For debugging purpose.
	void printDashes(int depth) {
		for (int i = 0; i < depth; i++) {
			std::cout << "-";
		}
	}
	virtual void printNode(int depth) = 0;
};

class ProgramNode : public SourceASTNode {
public:
	ProgramNode();
	//void addChild(ProcedureNode* procNode);
	//std::vector<SourceASTNode*> getChildren();

	void printNode(int depth = 1);

	friend class SourceAST;
};

class ProcedureNode : public SourceASTNode {
public:
	ProcedureNode(std::string& value);
	//void addChild(StmtListNode* stmtListNode);
	//std::vector<SourceASTNode*> getChildren();

	void printNode(int depth);

	friend class SourceAST;
};

class StmtListNode : public SourceASTNode {
public:
	StmtListNode();
	//void addChild(StmtNode* stmtNode);
	//std::vector<SourceASTNode*> getChildren();

	void printNode(int depth = 1);

	friend class SourceAST;
};

class StmtNode : public SourceASTNode {
public:
	StmtNode(std::string& value);
	//void addChild(SourceASTNode* node);
	//std::vector<SourceASTNode*> getChildren();

	void printNode(int depth = 1);

	friend class SourceAST;
};

class ReadNode : public StmtNode {
public:
	ReadNode(std::string& value);
	std::string getVarName();
	void printNode(int depth);

	friend class SourceAST;
};
