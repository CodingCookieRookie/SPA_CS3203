#pragma once

#include "SourceASTNode.h"
//#include "../Commons.h"

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