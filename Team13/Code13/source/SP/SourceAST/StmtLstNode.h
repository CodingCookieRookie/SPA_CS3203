#pragma once

#include "SourceASTNode.h"
#include "StmtNode.h"

class StmtLstNode : public SourceASTNode {
private:
	std::vector<StmtNode*> stmtNodes;
public:
	StmtLstNode();
	void addStmtNode(StmtNode* stmtNode);
	void process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) override;

	std::vector<StmtNode*> getStmtNodes();
	std::unordered_set<StmtIndex> getDirectStmtNodeIndices();
	std::unordered_set<StmtIndex> getAllStmtNodeIndices();
};
