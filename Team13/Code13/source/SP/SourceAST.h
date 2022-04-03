#pragma once

#include "SourceASTNode.h"

class SourceAST {
private:
	ProgramNode* root;

public:
	SourceAST(ProgramNode* root);
	~SourceAST();

	ProgramNode* getRoot();
	void process();

	/* Getters to relationship maps */
	FollowsMap getFollowsMap();
	ModifiesMap getModifiesMap();
	UsesMap getUsesMap();
	ParentChildMap getParentChildMap();
	CallStmtProcCalledMap getCallStmtProcCalledMap();

	/* Getters to entity maps */
	StmtNodes getStmtNodes();
	StmtTypeMap getStmtTypeMap();
	PatternMap getPatternMap();
	ConstSet getConstSet();
	ProcNameIndexMap getProcNameIndexMap();
	ProcNames getProcNames();
	ProcStmtMap getProcStmtMap();
	StmtProcMap getStmtProcMap();
};
