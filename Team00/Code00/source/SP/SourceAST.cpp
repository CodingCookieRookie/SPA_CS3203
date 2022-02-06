#include "SourceAST.h"

SourceAST::SourceAST(ProgramNode* root) : root(root) {}

ProgramNode* SourceAST::getRoot() {
	return root;
}

void SourceAST::printTree() {
	root->printNode();
}

SourceAST::~SourceAST() {
	delete root;
}
