#include "SourceAST.h"

SourceAST::SourceAST(ProgramNode* root) : root(root) {}

ProgramNode* SourceAST::getRoot() {
	return root;
}

SourceAST::~SourceAST() {
	delete root;
}

void SourceAST::process() {
	return root->process();
}

FollowsMap SourceAST::getFollowsMap() {
	return root->getRelationshipMaps().followsMap;
}

ModifiesMap SourceAST::getModifiesMap() {
	return root->getRelationshipMaps().modifiesMap;
}

UsesMap SourceAST::getUsesMap() {
	return root->getRelationshipMaps().usesMap;
}

ParentChildMap SourceAST::getParentChildMap() {
	return root->getRelationshipMaps().parentChildMap;
}

CallStmtProcCalledMap SourceAST::getCallStmtProcCalledMap() {
	return root->getRelationshipMaps().callStmtProcCalledMap;
}

StmtNodes SourceAST::getStmtNodes() {
	return root->getEntityMaps().stmtNodes;
}

StmtTypeMap SourceAST::getStmtTypeMap() {
	return root->getEntityMaps().stmtTypeMap;
}

PatternMap SourceAST::getPatternMap() {
	return root->getEntityMaps().patternMap;
}

ConstSet SourceAST::getConstSet() {
	return root->getEntityMaps().constSet;
}

ProcNameIndexMap SourceAST::getProcNameIndexMap() {
	return root->getEntityMaps().procNameIndexMap;
}

ProcNames SourceAST::getProcNames() {
	return root->getEntityMaps().procNames;
}

ProcStmtMap SourceAST::getProcStmtMap() {
	return root->getEntityMaps().procStmtMap;
}

StmtProcMap SourceAST::getStmtProcMap() {
	return root->getEntityMaps().stmtProcMap;
}
