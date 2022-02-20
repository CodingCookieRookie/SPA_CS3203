#include "SourceAST.h"

SourceAST::SourceAST(ProgramNode* root) : root(root) {}

ProgramNode* SourceAST::getRoot() {
	return root;
}

SourceAST::~SourceAST() {
	delete root;
}
