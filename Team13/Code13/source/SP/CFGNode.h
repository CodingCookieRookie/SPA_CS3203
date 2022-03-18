#pragma once

#include <unordered_set>

#include "../Common/Types.h"

class CFGNode {
private:
	std::unordered_set<StmtIndex> stmtIndices;
	std::unordered_set<CFGNode*> nextNodes;

public:
	CFGNode();
	CFGNode(std::unordered_set<StmtIndex> stmtIndices);

	void addNext(CFGNode* node);
	std::unordered_set<StmtIndex> getStmtIndices();
	std::unordered_set<CFGNode*> getNextNodes();
};
