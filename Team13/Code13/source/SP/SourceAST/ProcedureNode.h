#pragma once

#include "SourceASTNode.h"
#include "StmtLstNode.h"

class ProcedureNode : public SourceASTNode {
private:
	std::string procName;
	StmtLstNode* stmtLstNode;
	ProcIndex procIndex;

	void bidirectionalPopulateProcAndStmt(ProcStmtMap& procStmtMap, StmtProcMap& stmtProcMap, std::unordered_set<StmtIndex>& stmtIndices);
	void bidirectionalPopulateProcNameAndIndex(ProcNameToIndexMap& procNameToIndexMap, SortedProcIndexToNameMap& sortedProcIndexToNameMap);
public:
	ProcedureNode(std::string procName);
	void setProcIndex(ProcIndex procIndex);
	void addStmtLst(StmtLstNode* stmtLstNode);
	void process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) override;

	StmtLstNode* getStmtLstNode();
	std::string getProcName();
};
