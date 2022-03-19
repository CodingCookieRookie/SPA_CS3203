#include "CFG.h"

CFG::CFG(CFGNode* head, CFGNode* tail) : head(head), tail(tail) {}

size_t CFG::size() {
	size_t res = 1;
	CFGNode* head = this->getHead();
	std::unordered_set<CFGNode*> cfgNodes;
	cfgNodes.insert(head);
	res += getSizeHelper(head, cfgNodes);
	return res;
}

size_t CFG::getSizeHelper(CFGNode*& currNode, std::unordered_set<CFGNode*>& cfgNodes) {
	size_t res = 0;
	for (CFGNode* node : currNode->getNextNodes()) {
		if (cfgNodes.find(node) == cfgNodes.end()) {
			cfgNodes.insert(node);
			res += 1;
			res += getSizeHelper(node, cfgNodes);
		}
	}
	return res;
}

CFGNode* CFG::getHead() {
	return head;
}

CFGNode* CFG::getTail() {
	return tail;
}

void CFG::addToEnd(CFGNode* node) {
	tail->addNext(node);
	tail = node;
}
