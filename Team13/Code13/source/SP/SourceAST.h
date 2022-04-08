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
	RelationshipMap getFollowsMap();
	RelationshipMap getModifiesMap();
	RelationshipMap getUsesMap();
	RelationshipMap getParentChildMap();
	RelationshipMap getCallStmtToProcIndexCalledMap();
	RelationshipMap getProcIndexCallerToProcIndexCalledMap();

	/* Getters to entity maps */
	StmtNodes getStmtNodes();
	StmtTypeMap getStmtTypeMap();
	PatternMap getPatternMap();
	ConstSet getConstSet();
	ProcNameToIndexMap getProcNameToIndexMap();
	SortedProcIndexToNameMap getSortedProcIndexToNameMap();
	ProcStmtMap getProcStmtMap();
	StmtProcMap getStmtProcMap();
	VarNameToIndexMap getVarNameToIndexMap();
	SortedVarIndexToNameMap getSortedVarIndexToNameMap();
};
