#pragma once

#include <unordered_map>
#include <unordered_set>

#include "../Common/Types.h"

class CFG {
private:
	std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> cfgTable;

public:
	CFG();
	~CFG();
	void insert(StmtIndex predecessor, StmtIndex successor);
	std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> getCFGTable();
};
