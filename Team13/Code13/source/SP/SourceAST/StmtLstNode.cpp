#include "StmtLstNode.h"

StmtLstNode::StmtLstNode() : SourceASTNode() {}

void StmtLstNode::addStmtNode(StmtNode* stmtNode) {
	stmtNodes.push_back(stmtNode);
}

void StmtLstNode::process(RelationshipMaps& relationshipMaps, EntityMaps& entityMaps) {
	StmtIndex prevIndex = 0;
	for (StmtNode* stmtNode : getStmtNodes()) {
		stmtNode->process(prevIndex, relationshipMaps, entityMaps);
		prevIndex = stmtNode->getStmtIdx();
	}
}

std::vector<StmtNode*> StmtLstNode::getStmtNodes() {
	return stmtNodes;
}

std::unordered_set<StmtIndex> StmtLstNode::getDirectStmtNodeIndices() {
	std::unordered_set<StmtIndex> stmtNodeIndices;
	for (StmtNode* stmtNode : getStmtNodes()) {
		stmtNodeIndices.insert(stmtNode->getStmtIdx());
	}

	return stmtNodeIndices;
}

std::unordered_set<StmtIndex> StmtLstNode::getAllStmtNodeIndices() {
	std::unordered_set<StmtIndex> stmtNodeIndices = getDirectStmtNodeIndices();
	for (StmtNode* stmtNode : getStmtNodes()) {
		for (StmtLstNode* nestedStmtLstNode : stmtNode->getChildStmtLst()) {
			std::unordered_set<StmtIndex> nestedIndices = nestedStmtLstNode->getAllStmtNodeIndices();
			stmtNodeIndices.insert(nestedIndices.begin(), nestedIndices.end());
		}
	}

	return stmtNodeIndices;
}
