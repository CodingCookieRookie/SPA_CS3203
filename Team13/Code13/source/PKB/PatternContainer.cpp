#include "./PatternContainer.h"

PatternContainer::PatternContainer() {}

void PatternContainer::insert(StmtIndex& stmtIdx, VarIndex& varIdx) {
	containerVarTable[varIdx].insert(stmtIdx);
}
std::vector<StmtIndex> PatternContainer::getStmtsFromVar(VarIndex& varIndex) {
	std::unordered_set<StmtIndex> stmtSet = containerVarTable[varIndex];
	return std::vector<StmtIndex>(stmtSet.begin(), stmtSet.end());
}

std::tuple<std::vector<VarIndex>, std::vector<StmtIndex>> PatternContainer::getAll() {
	std::vector<StmtIndex> stmtIndices;
	std::vector<VarIndex> varIndices;

	for (auto& containerVarInfo : containerVarTable) {
		VarIndex varIdx = containerVarInfo.first;
		for (auto& stmtIdx : containerVarInfo.second) {
			stmtIndices.push_back(stmtIdx);
			varIndices.push_back(varIdx);
		}
	}

	return std::make_tuple(varIndices, stmtIndices);
}
