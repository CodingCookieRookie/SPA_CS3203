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

RelationshipMap SourceAST::getFollowsMap() {
	return root->getRelationshipMaps().followsMap;
}

RelationshipMap SourceAST::getModifiesMap() {
	return root->getRelationshipMaps().modifiesMap;
}

RelationshipMap SourceAST::getUsesMap() {
	return root->getRelationshipMaps().usesMap;
}

RelationshipMap SourceAST::getParentChildMap() {
	return root->getRelationshipMaps().parentChildMap;
}

RelationshipMap SourceAST::getCallStmtToProcIndexCalledMap() {
	return root->getRelationshipMaps().callStmtToProcIndexCalledMap;
}

RelationshipMap SourceAST::getProcIndexCallerToProcIndexCalledMap() {
	return root->getRelationshipMaps().procIndexCallerToProcIndexCalledMap;
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

ProcNameToIndexMap SourceAST::getProcNameToIndexMap() {
	return root->getEntityMaps().procNameToIndexMap;
}

SortedProcIndexToNameMap SourceAST::getSortedProcIndexToNameMap() {
	return root->getEntityMaps().sortedProcIndexToNameMap;
}

ProcStmtMap SourceAST::getProcStmtMap() {
	return root->getEntityMaps().procStmtMap;
}

StmtProcMap SourceAST::getStmtProcMap() {
	return root->getEntityMaps().stmtProcMap;
}

VarNameToIndexMap SourceAST::getVarNameToIndexMap() {
	return root->getEntityMaps().varNameToIndexMap;
}

SortedVarIndexToNameMap SourceAST::getSortedVarIndexToNameMap() {
	return root->getEntityMaps().sortedVarIndexToNameMap;
}
