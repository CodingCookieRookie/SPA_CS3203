#pragma once

#include <set>
#include <unordered_set>

#include "../Common/Types.h"

class CFGNode {
private:
	std::set<StmtIndex> stmtIndices;
	std::unordered_set<CFGNode*> nextNodes;

public:
	CFGNode();
	CFGNode(std::set<StmtIndex> stmtIndices);

	void addNext(CFGNode* node);
	std::set<StmtIndex> getStmtIndices();
	std::unordered_set<CFGNode*> getNextNodes();
};
