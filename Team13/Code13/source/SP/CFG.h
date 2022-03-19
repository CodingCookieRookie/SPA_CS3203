#pragma once

#include "CFGNode.h"

class CFG {
private:
	CFGNode* head;
	CFGNode* tail;
	size_t getSizeHelper(CFGNode*& node, std::unordered_set<CFGNode*>& cfgNodes);

public:
	CFG(CFGNode* head, CFGNode* tail);

	size_t size();
	CFGNode* getHead();
	CFGNode* getTail();
	void addToEnd(CFGNode* node);
};
