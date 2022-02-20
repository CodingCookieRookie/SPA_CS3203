#pragma once

#ifndef SOURCEAST_H
#define SOURCEAST_H

#include "SourceASTNode.h"

class SourceAST {
private:
	ProgramNode* root;
public:
	SourceAST(ProgramNode* root);
	~SourceAST();

	ProgramNode* getRoot();
};

#endif
