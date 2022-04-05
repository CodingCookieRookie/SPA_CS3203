#pragma once

#include <tuple>
#include <unordered_map>
#include <vector>

#include "../Common/Types.h"
#include "./Attribute.h"
#include "./BidirectionalTable/BidirectionalIndexTable.h"

class PatternContainer {
protected:
	std::unordered_map<VarIndex, std::unordered_set<StmtIndex>> containerVarTable;

public:
	PatternContainer();
	void insert(StmtIndex& stmtIdx, VarIndex& varIdx);
	std::vector<StmtIndex> getStmtsFromVar(VarIndex& varIndex);
	std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> getAll();
};
