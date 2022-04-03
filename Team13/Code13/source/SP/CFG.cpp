#include "CFG.h"

CFG::CFG() {}

CFG::~CFG() {}

void CFG::insert(StmtIndex predecessor, StmtIndex successor) {
	cfgTable[predecessor].insert(successor);
}

std::unordered_map<StmtIndex, std::unordered_set<StmtIndex>> CFG::getCFGTable() {
	return cfgTable;
}
