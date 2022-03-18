#include "CFGNode.h"

CFGNode::CFGNode() {}
CFGNode::CFGNode(std::unordered_set<StmtIndex> stmtIndices) : stmtIndices(stmtIndices) {}

void CFGNode::addNext(CFGNode* node) {
	nextNodes.insert(node);
}

std::unordered_set<StmtIndex> CFGNode::getStmtIndices() {
	return stmtIndices;
}

std::unordered_set<CFGNode*> CFGNode::getNextNodes() {
	return nextNodes;
}
