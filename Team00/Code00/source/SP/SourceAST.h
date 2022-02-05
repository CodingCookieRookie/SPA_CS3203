#pragma once

#include "SourceASTNode.h"

class SourceAST {
private:
	ProgramNode* root;
public:
	SourceAST(ProgramNode* root);
	~SourceAST();

	ProgramNode* getRoot();

	// For debugging purposes only
	void printTree();
};
